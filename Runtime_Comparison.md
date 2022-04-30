We tested the runtime of our tests starting at the initialization of the convulation object and after the reset of the object.

As shown by the times below, the initialization of the partitioned fast convolution in the frequency domain takes slightly longer to initialize and clear, 
however, the actual processing is much faster as shown by the test using different block sizes.

FastConv.Identity_Time Test
IdentityTime = 2 micro sec

FastConv.FlushBuffer_Time Test
FlushBufferTime = 2 micro sec

FastConv.BlockSize_Time Test
BlockSizeTime = 119145 micro sec

FastConv.Identity_Freq Test
IdentityFreq = 97 micro sec

FastConv.FlushBuffer_Freq Test
FlushBufferFreq = 7 micro sec

FastConv.BlockSize_Freq Test
BlockSizeFreq = 8075 micro sec
