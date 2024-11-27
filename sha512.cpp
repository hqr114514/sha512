#include "sha512.h"

#define S(x,y) ((((ull)x)>>(y))|((ull)x)<<(64-(y)))
#define swap64(x) ((x >> 56) | ((x >> 40) & 0xff00ull) | ((x >> 24) & 0xff0000ull) | ((x >> 8) & 0xff000000ull) | ((x << 8) & 0xff00000000ull) | ((x << 24) & 0xff0000000000ull) | ((x << 40) & 0xff000000000000ull) | (x << 56))
#define swap32(x) ((x >> 24) | ((x >> 8) & 0xff00) | ((x << 8) & 0xff0000) | (x << 24))
#define Ch(x,y,z) (((x) & (y)) ^ (~(x) & (z)))
#define Ma(x,y,z) (((x)&(y))^((x)&(z))^((y)&(z)))
#define S0(x) (S((x),28)^S((x),34)^S((x),39))
#define S1(x) (S((x),14)^S((x),18)^S((x),41))
#define o0(x) (S((x),1)^S((x),8)^((x)>>7))
#define o1(x) (S((x),19)^S((x),61)^((x)>>6))

const ull k[80] = {
	0x428a2f98d728ae22ULL, 0x7137449123ef65cdULL, 0xb5c0fbcfec4d3b2fULL, 0xe9b5dba58189dbbcULL,
	0x3956c25bf348b538ULL, 0x59f111f1b605d019ULL, 0x923f82a4af194f9bULL, 0xab1c5ed5da6d8118ULL,
	0xd807aa98a3030242ULL, 0x12835b0145706fbeULL, 0x243185be4ee4b28cULL, 0x550c7dc3d5ffb4e2ULL,
	0x72be5d74f27b896fULL, 0x80deb1fe3b1696b1ULL, 0x9bdc06a725c71235ULL, 0xc19bf174cf692694ULL,
	0xe49b69c19ef14ad2ULL, 0xefbe4786384f25e3ULL, 0x0fc19dc68b8cd5b5ULL, 0x240ca1cc77ac9c65ULL,
	0x2de92c6f592b0275ULL, 0x4a7484aa6ea6e483ULL, 0x5cb0a9dcbd41fbd4ULL, 0x76f988da831153b5ULL,
	0x983e5152ee66dfabULL, 0xa831c66d2db43210ULL, 0xb00327c898fb213fULL, 0xbf597fc7beef0ee4ULL,
	0xc6e00bf33da88fc2ULL, 0xd5a79147930aa725ULL, 0x06ca6351e003826fULL, 0x142929670a0e6e70ULL,
	0x27b70a8546d22ffcULL, 0x2e1b21385c26c926ULL, 0x4d2c6dfc5ac42aedULL, 0x53380d139d95b3dfULL,
	0x650a73548baf63deULL, 0x766a0abb3c77b2a8ULL, 0x81c2c92e47edaee6ULL, 0x92722c851482353bULL,
	0xa2bfe8a14cf10364ULL, 0xa81a664bbc423001ULL, 0xc24b8b70d0f89791ULL, 0xc76c51a30654be30ULL,
	0xd192e819d6ef5218ULL, 0xd69906245565a910ULL, 0xf40e35855771202aULL, 0x106aa07032bbd1b8ULL,
	0x19a4c116b8d2d0c8ULL, 0x1e376c085141ab53ULL, 0x2748774cdf8eeb99ULL, 0x34b0bcb5e19b48a8ULL,
	0x391c0cb3c5c95a63ULL, 0x4ed8aa4ae3418acbULL, 0x5b9cca4f7763e373ULL, 0x682e6ff3d6b2b8a3ULL,
	0x748f82ee5defb2fcULL, 0x78a5636f43172f60ULL, 0x84c87814a1f0ab72ULL, 0x8cc702081a6439ecULL,
	0x90befffa23631e28ULL, 0xa4506cebde82bde9ULL, 0xbef9a3f7b2c67915ULL, 0xc67178f2e372532bULL,
	0xca273eceea26619cULL, 0xd186b8c721c0c207ULL, 0xeada7dd6cde0eb1eULL, 0xf57d4f7fee6ed178ULL,
	0x06f067aa72176fbaULL, 0x0a637dc5a2c898a6ULL, 0x113f9804bef90daeULL, 0x1b710b35131c471bULL,
	0x28db77f523047d84ULL, 0x32caab7b40c72493ULL, 0x3c9ebe0a15c9bebcULL, 0x431d67c49c100d4cULL,
	0x4cc5d4becb3e42b6ULL, 0x597f299cfc657e2aULL, 0x5fcb6fab3ad6faecULL, 0x6c44198c4a475817ULL
};

ull H[8] = {
	0x6a09e667f3bcc908ULL,
	0xbb67ae8584caa73bULL,
	0x3c6ef372fe94f82bULL,
	0xa54ff53a5f1d36f1ULL,
	0x510e527fade682d1ULL,
	0x9b05688c2b3e6c1fULL,
	0x1f83d9abfb41bd6bULL,
	0x5be0cd19137e2179ULL
};

void block(uc data[128]) {
	ull w[80] = { 0 };//将1024bit转换为16个64bit的无符号整数
	//转换:
	for (int t = 0, j = 0; t < 16; t++, j += 8) w[t] = (ull)((ull(((ull)data[j]) << 56ull)) | (ull(((ull)data[j + 1]) << 48ull)) |
	(ull(((ull)data[j + 2]) << 40ull)) | (ull(((ull)data[j + 3]) << 32ull)) | (ull(((ull)data[j + 4]) << 24ull)) |
	(ull(((ull)data[j + 5]) << 16ull)) | (ull(((ull)data[j + 6]) << 8ull)) | (ull((ull)data[j + 7])));
	//扩充:
	for (int t = 16; t < 80; t++) w[t] = o1(w[t - 2]) + w[t - 7] + o0(w[t - 15]) + w[t - 16];
	ull a = H[0];
	ull b = H[1];
	ull c = H[2];
	ull d = H[3];
	ull e = H[4];
	ull f = H[5];
	ull g = H[6];
	ull h = H[7];
	/*for (int i = 0; i < 80; i++) {
		printf("%08x", w[i]>>32);
		printf("%08x", w[i] & 0xffffffffull);
		if (i % 2 == 1) cout << endl;
	}*/
	for (int i = 0; i < 80; i++) {
		ull T1 = h + S1(e) + Ch(e, f, g) + k[i] + w[i];
		ull T2 = S0(a) + Ma(a, b, c);
		h = g;
		g = f;
		f = e;
		e = d + T1;
		d = c;
		c = b;
		b = a;
		a = T1 + T2;
	}
	H[0] += a;
	H[1] += b;
	H[2] += c;
	H[3] += d;
	H[4] += e;
	H[5] += f;
	H[6] += g;
	H[7] += h;
	return;
}

void sha512(const char* FileName, uc* out) {
	ifstream fin(FileName, ios::binary);
	if (!fin) return;
	fin.seekg(0, ios::end);//文件指针移动至最后
	ull siz = fin.tellg();//获取总长度
	fin.seekg(0, ios::beg);//文件指针移到开头
	uc data[128] = { 0 };//128B = 1024bit Data
	while (fin.read((char*)data, 128)) {//循环处理1024bit的块
		//处理块:
		block(data);
		memset(data, 0, 128 * sizeof(uc));
	}
	//处理剩余
	ui rest = fin.gcount();
	data[rest] = 0x80;//填1
	for (int i = rest + 1; i < 128; i++) data[i] = 0;//填0
	if ((rest + 1) * 8 > 896) {
		//要分两块
		block(data);
		memset(data, 0, 128 * sizeof(uc));
	}
	
	for (int i = 0; i < 8; i++) {//16byte(128bit) length information
		data[120 + i] = (siz * 8) >> ((8 - i - 1) * 8);
	}
	/*cout << "size:" << siz * 8 << endl;
	for (int i = 0; i < 128; i++) {
		printf("%02x", data[i]);
		if (i % 16 == 15) cout << endl;
	}*/
	block(data);
	fin.close();
	for (int i = 0, j = 0; i < 64; i++, j += 8) {
		if (j == 64) j = 0;
		out[i] = (((ull)swap64(H[i / 8])) >> j) & 0xff;
	}
	return;
}