#!/usr/bin/env node
/**
 * lastdb tcp client for the intentionally tiny binary protocol on port 51515.
 * Inputs are int32 username, int32 password, int32 cipherkey, int32 color, UTF-8 tenant/key/value.
 * Outputs are stdout payload bytes for CLI calls and exported async functions for app code.
 */
import net from 'node:net';
import process from 'node:process';
import {once} from 'node:events';
import {pathToFileURL} from 'node:url';

var MAGIC = 0x4c444231;
var OK = 0;
var MAX_FRAME = 64 * 1024 * 1024;
var OP = new Map([
    ['get', 1],
    ['put', 2],
    ['del', 3],
    ['scan', 4],
    ['search', 5],
    ['tail', 6],
    ['closest', 7],
    ['count', 8],
    ['sum', 9],
    ['incr', 10],
    ['grant', 11],
    ['take', 12],
    ['putnx', 13],
    ['cas', 14],
    ['decay', 15],
    ['batch', 16],
]);

export async function open(host, username, password, cipherkey, port = 51515) {
    var socket = net.createConnection({host, port});
    socket.setNoDelay(true);
    socket.setTimeout(30000);
    await Promise.race([
        once(socket, 'connect'),
        once(socket, 'error').then(throwFirst),
        once(socket, 'timeout').then(throwTimeout),
    ]);
    return {socket, username: i32(username), password: i32(password), cipherkey: i32(cipherkey), rx: Buffer.alloc(0)};
}

export async function close(client) {
    client.socket.end();
    await once(client.socket, 'close');
}

export async function get(client, color, tenant, key) {
    return request(client, 'get', color, tenant, key, '');
}

export async function put(client, color, tenant, key, value) {
    return request(client, 'put', color, tenant, key, value);
}

export async function drop(client, color, tenant, key) {
    return request(client, 'del', color, tenant, key, '');
}

export async function scan(client, color, tenant, prefix = '') {
    return request(client, 'scan', color, tenant, prefix, '');
}

export async function search(client, color, tenant, words) {
    return request(client, 'search', color, tenant, '', Array.isArray(words) ? words.join('\t') : words);
}

export async function tail(client, color, offset = 0) {
    return request(client, 'tail', color, '', '', String(offset));
}

export async function* follow(client, color, offset = 0) {
    while (true) {
        var res = await request(client, 'tail', color, '', '', String(offset));
        var str = res.toString();
        var lines = str.split('\n');
        for (var i = 0; i < lines.length - 1; i++) {
            var parts = lines[i].split('\t');
            offset = Number(parts[0]);
            yield parts;
        }
        if (!res.hasMore && lines.length < 2) {
            await new Promise(r => setTimeout(r, 500));
        }
    }
}

export async function closest(client, color, tenant, keyOrType, typeOrVector) {
    return request(client, 'closest', color, tenant, keyOrType, typeOrVector);
}

export async function count(client, color, tenant, prefix = '', threshold = 1.0) {
    return request(client, 'count', color, tenant, prefix, String(threshold));
}

export async function sum(client, color, tenant, prefix = '', threshold = 1.0) {
    return request(client, 'sum', color, tenant, prefix, String(threshold));
}

export async function incr(client, color, tenant, key, delta) {
    return request(client, 'incr', color, tenant, key, String(delta));
}

export async function take(client, color, tenant, key) {
    return request(client, 'take', color, tenant, key, '');
}

export async function putnx(client, color, tenant, key, value) {
    return request(client, 'putnx', color, tenant, key, value);
}

export async function cas(client, color, tenant, key, old_val, new_val) {
    return request(client, 'cas', color, tenant, key, old_val + '\t' + new_val);
}

export async function grant(client, color, user, password, permissions) {
    return request(client, 'grant', color, '', String(user), i32(password) + ',' + i32(permissions));
}

export async function decay(client, color, tenant, key, half_life, now, delta) {
    return request(client, 'decay', color, tenant, key, String(half_life) + '\t' + String(now) + '\t' + String(delta));
}

export async function batch(client, color, tenant, pairs) {
    var lines = pairs.map(p => p[0] + '\t' + p[1]).join('\n');
    return request(client, 'batch', color, tenant, '', lines);
}

export async function request(client, op, color, tenant, key, value) {
    await writeFrame(client, encodeRequest(client, op, color, tenant, key, value));
    return decodeResponse(client, await readFrame(client));
}

function encodeRequest(client, op, color, tenant, key, value) {
    var tb = Buffer.from(tenant);
    var kb = Buffer.from(key);
    var vb = Buffer.isBuffer(value) ? value : Buffer.from(value);
    var body = Buffer.alloc(32 + tb.length + kb.length + vb.length);
    body.writeUInt32BE(MAGIC, 0);
    body.writeInt32BE(opcode(op), 4);
    body.writeInt32BE(client.username, 8);
    body.writeInt32BE(client.password, 12);
    body.writeInt32BE(i32(color), 16);
    body.writeUInt32BE(tb.length, 20);
    body.writeUInt32BE(kb.length, 24);
    body.writeUInt32BE(vb.length, 28);
    tb.copy(body, 32);
    kb.copy(body, 32 + tb.length);
    vb.copy(body, 32 + tb.length + kb.length);
    return crypt(body, client.cipherkey);
}

async function writeFrame(client, body) {
    if (body.length > MAX_FRAME) {
        throw new Error('frame too large: ' + body.length);
    }
    var frame = Buffer.alloc(4 + body.length);
    frame.writeUInt32BE(body.length, 0);
    body.copy(frame, 4);
    if (client.socket.write(frame)) {
        return;
    }
    await Promise.race([
        once(client.socket, 'drain'),
        once(client.socket, 'error').then(throwFirst),
        once(client.socket, 'timeout').then(throwTimeout),
    ]);
}

async function readFrame(client) {
    while (client.rx.length < 4) {
        var chunk = await readChunk(client);
        client.rx = Buffer.concat([client.rx, chunk]);
    }

    var len = client.rx.readUInt32BE(0);
    if (len > MAX_FRAME) {
        throw new Error('frame too large: ' + len);
    }
    while (client.rx.length < 4 + len) {
        var chunk = await readChunk(client);
        client.rx = Buffer.concat([client.rx, chunk]);
    }

    var body = client.rx.subarray(4, 4 + len);
    client.rx = client.rx.subarray(4 + len);
    return crypt(body, client.cipherkey);
}

async function readChunk(client) {
    var chunk = client.socket.read();
    if (chunk) {
        return chunk;
    }
    var got = await Promise.race([
        once(client.socket, 'readable'),
        once(client.socket, 'error').then(throwFirst),
        once(client.socket, 'timeout').then(throwTimeout),
        once(client.socket, 'end').then(throwEnd),
    ]);
    void got;
    return readChunk(client);
}

function decodeResponse(client, body) {
    void client;
    if (body.length < 16) {
        throw new Error('short response: ' + body.length);
    }
    if (body.readUInt32BE(0) !== MAGIC) {
        throw new Error('bad magic: ' + body.readUInt32BE(0));
    }
    var status = body.readInt32BE(4);
    var n = body.readUInt32BE(12);
    if (body.length !== 16 + n) {
        throw new Error('bad response length: ' + JSON.stringify({frame: body.length, payload: n}));
    }
    var payload = body.subarray(16);
    if (status !== OK && status !== 4) {
        throw new Error(payload.toString() || ('lastdb status ' + status));
    }
    payload.hasMore = status === 4;
    return payload;
}

async function main() {
    var a = process.argv.slice(2);
    if (a.length < 5 || a[0] === '-h' || a[0] === '--help') {
        return usage();
    }

    var client = await open(a[0], parseI32(a[1]), parseI32(a[2]), parseI32(a[3]));
    try {
        process.stdout.write(await cli(client, a.slice(4)));
    } finally {
        client.socket.end();
    }
}

async function cli(client, a) {
    if (a[0] === 'get' && a.length === 4) {
        return get(client, parseI32(a[1]), a[2], a[3]);
    }
    if (a[0] === 'put' && a.length === 5) {
        return put(client, parseI32(a[1]), a[2], a[3], a[4]);
    }
    if (a[0] === 'del' && a.length === 4) {
        return drop(client, parseI32(a[1]), a[2], a[3]);
    }
    if (a[0] === 'scan' && (a.length === 3 || a.length === 4)) {
        return scan(client, parseI32(a[1]), a[2], a[3] ?? '');
    }
    if (a[0] === 'search' && a.length >= 4) {
        return search(client, parseI32(a[1]), a[2], a.slice(3));
    }
    if (a[0] === 'tail' && (a.length === 2 || a.length === 3)) {
        return tail(client, parseI32(a[1]), a[2] ?? 0);
    }
    if (a[0] === 'follow' && (a.length === 2 || a.length === 3)) {
        for await (var parts of follow(client, parseI32(a[1]), a[2] ?? 0)) {
            process.stdout.write(parts.join('\t') + '\n');
        }
        return '';
    }
    if (a[0] === 'closest' && a.length === 5) {
        return closest(client, parseI32(a[1]), a[2], a[3], a[4]);
    }
    if (a[0] === 'count' && (a.length >= 3 && a.length <= 5)) {
        return count(client, parseI32(a[1]), a[2], a[3] ?? '', a[4] ? Number(a[4]) : 1.0);
    }
    if (a[0] === 'sum' && (a.length >= 3 && a.length <= 5)) {
        return sum(client, parseI32(a[1]), a[2], a[3] ?? '', a[4] ? Number(a[4]) : 1.0);
    }
    if (a[0] === 'incr' && a.length === 5) {
        return incr(client, parseI32(a[1]), a[2], a[3], a[4]);
    }
    if (a[0] === 'take' && a.length === 4) {
        return take(client, parseI32(a[1]), a[2], a[3]);
    }
    if (a[0] === 'putnx' && a.length === 5) {
        return putnx(client, parseI32(a[1]), a[2], a[3], a[4]);
    }
    if (a[0] === 'cas' && a.length === 6) {
        return cas(client, parseI32(a[1]), a[2], a[3], a[4], a[5]);
    }
    if (a[0] === 'grant' && a.length === 5) {
        return grant(client, parseI32(a[1]), parseI32(a[2]), parseI32(a[3]), parseI32(a[4]));
    }
    if (a[0] === 'decay' && a.length === 7) {
        return decay(client, parseI32(a[1]), a[2], a[3], a[4], a[5], a[6]);
    }
    if (a[0] === 'batch' && a.length >= 5 && a.length % 2 === 1) {
        var pairs = [];
        for (var i = 3; i < a.length; i += 2) pairs.push([a[i], a[i+1]]);
        return batch(client, parseI32(a[1]), a[2], pairs);
    }
    usage();
}

function opcode(op) {
    if (!OP.has(op)) {
        throw new Error('bad op: ' + op);
    }
    return OP.get(op);
}

function crypt(buf, key) {
    var k = i32(key);
    if (!k) {
        return buf;
    }

    for (var i = 0; i < buf.length; i++) {
        k ^= k << 13;
        k ^= k >>> 17;
        k ^= k << 5;
        buf[i] ^= (k & 255);
    }
    return buf;
}

function i32(x) {
    if (!Number.isInteger(x) || x < -2147483648 || x > 2147483647) {
        throw new Error('not int32: ' + x);
    }
    return x;
}

function parseI32(s) {
    var text = String(s);
    var start = text[0] === '-' ? 1 : 0;
    if (start === text.length) {
        throw new Error('not int32: ' + s);
    }
    for (var i = start; i < text.length; i++) {
        var c = text.charCodeAt(i);
        if (c < 48 || c > 57) {
            throw new Error('not int32: ' + s);
        }
    }
    return i32(Number(text));
}

function throwFirst(args) {
    throw args[0];
}

function throwTimeout() {
    throw new Error('socket timeout');
}

function throwEnd() {
    throw new Error('socket ended');
}

function usage() {
    process.stderr.write([
        'usage: client.mjs HOST USER PASS CIPHER CMD ...',
        '  get COLOR TENANT KEY',
        '  put COLOR TENANT KEY VALUE',
        '  del COLOR TENANT KEY',
        '  scan COLOR TENANT [PREFIX]',
        '  search COLOR TENANT WORD...',
        '  tail COLOR [OFFSET]',
        '  closest COLOR TENANT KEY TYPE',
        '  count COLOR TENANT [PREFIX] [THRESHOLD]',
        '  sum COLOR TENANT [PREFIX] [THRESHOLD]',
        '  incr COLOR TENANT KEY DELTA',
        '  take COLOR TENANT KEY',
        '  putnx COLOR TENANT KEY VALUE',
        '  cas COLOR TENANT KEY OLD NEW',
        '  decay COLOR TENANT KEY HALF_LIFE NOW DELTA',
        '  batch COLOR TENANT KEY1 VAL1 [KEY2 VAL2 ...]',
        '  grant COLOR USER PASS PERMS',
        '',
    ].join('\n'));
    process.exit(2);
}

function onMainError(error) {
    console.error(error);
    process.exit(1);
}

if (process.argv[1] && import.meta.url === pathToFileURL(process.argv[1]).href) {
    main().catch(onMainError);
}
