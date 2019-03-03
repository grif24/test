# dream
reference implementation of the sponge function dream, following the style of BLAKE2's reference implementation (i
enjoyed the design). provides two instances: dream[r, c] -> dream[1280,256] and dream[1024,512]. this function was designed with the two main goals of high performance for long messages, and comfortable security margins.

make tests && ./tests from the root directory will build a quick sanity check

similarly, make avx_tests && avx_tests will build the avx implementation (compare outputs of both versions to ensure accuracy). avx2 required, avx512f + avx512vl for the most optimized version (single instruction packed 64 bit rotate added).

(not updated to test ver) see [this](https://docs.google.com/document/d/1qwd6pyg_UqVCj62hyIIiU8OoCUr5BeuD3Yn_uVZ18tw/edit?usp=sharing) link for a
more in-depth description (stream of consciousness) of the algorithm 

# hash
dream128 provides 32 byte digests, while dream256 provides 64. these functions have a security level of 2<sup>128</sup>
and 2<sup>256</sup> respectively against collision, preimage, and second preimage.

# authenticated encryption
single pass authenticated encryption is implemented via the wrap and unwrap functions, following the SpongeWrap mode
described by Keccak's authors. dream128 supports keys of 16 bytes and associated data of up to 159 bytes, while dream256 supports keys of 32 bytes and associated data of up to 127 bytes.

# todo
xor of round constant into avx register probably slow (store forwading?)

fix short messages...
