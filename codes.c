/* 

Code Snippet showing the overlapping of the Computation 
and Communication

*/
vbx_word_t *v_a = vbx_sp_malloc( M*sizeof(vbx_word_t) );
vbx_word_t *v_b = vbx_sp_malloc( M*sizeof(vbx_word_t) );
vbx_set_vl( M );
for( i = 0; i < M*N; i += M ){
    vbx_dma_to_vector( v_a, a+i, M*sizeof(vbx_word_t) );
    vbx( VVW, VMUL, v_b, v_a, v_a );
    vbx( VVW, VMUL, v_b, v_b, v_a );
    vbx_dma_to_host( b+i, v_b, M*sizeof(vbx_word_t) );
}


/* 

Code Snippet showing the overlapping of the Computation and Communication using 
double buffering strategy.

*/

vbx_word_t *v_a0 = vbx_sp_malloc( M*sizeof(vbx_word_t) );
vbx_word_t *v_a1 = vbx_sp_malloc( M*sizeof(vbx_word_t) );
vbx_word_t *v_b0 = vbx_sp_malloc( M*sizeof(vbx_word_t) );
vbx_word_t *v_b1 = vbx_sp_malloc( M*sizeof(vbx_word_t) );
vbx_word_t *v_tmp;

vbx_set_vl( M );
vbx_dma_to_vector( v_a0, a, M*sizeof(vbx_word_t) );
for( i = 0; i < M*N; i += M ){
    if( i < M*N-M ){
        vbx_dma_to_vector( v_a1, a+i+M, M*sizeof(vbx_word_t) );
    }
    vbx( VVW, VMUL, v_b0, v_a0, v_a0 );
    vbx( VVW, VMUL, v_b0, v_b0, v_a0 );
    vbx_dma_to_host( b+i, v_b0, M*sizeof(vbx_word_t) );
    
    //Swap buffers by changing pointers
    v_tmp = v_a0; v_a0 = v_a1; v_a1 = v_tmp;
    v_tmp = v_b0; v_b0 = v_b1; v_b1 = v_tmp;
}


/*

Simple example to show the programming methodology and the overall software process
involved.

*/

int input_data[512] = { 0,1,2,3, 10, 11, 12, 13, ... ,511 };

int multiplier = 4;



I). Allocate vectors in local scratchpad

vbx_word_t* vdata;

v_data = vbx_sp_malloc( 512*4 );                                  // 512 words,in scratchpad



II). Moving data from main memory DDR3 to local scratchpad

vbx_dcache_flush( data, 512*4 );                                 //remove data from cache

vbx_dma_to_vector( vdata, data, 512*4 );                         // copy from 'data'


III). Set the vector length of register

vbx_set_vl( 512 );                                               // No of elements



IV). Performing vector operation

vbx( SVW, VMUL, vdata, multiplier, vdata );                     //instruction



V). Moving data from local scratchpad to DDR

vbx_dma_to_host( data, vdata, 512*4 );                         // copy data back

vbx_sync();                                                    // wait vector/DMA to finish




//hsi command helps to switch to a TCL shell
hsi
hsi% set hw_design [open_hw_design system.hdf]
hsi% generate_app -hw $hw_design -os standalone -proc 
ps7_cortexa9_0 -app zynq_fsbl -compile -sw fsbl -dir mxp_fsbl
hsi% quit




// change the following content
"sdboot=echo Copying Linux from SD to RAM...;" \
"mmcinfo;" \
"fatload mmc 0 0x3000000 ${kernel_image};" \
"fatload mmc 0 0x2A00000 ${devicetree_image};" \
"fatload mmc 0 0x2000000 ${ramdisk_image};" \
"bootm 0x3000000 0x2000000 0x2A00000\0" \

// to the following content
"sdboot=echo Copying Linux from SD to RAM...;" \
"mmcinfo;" \
"fatload mmc 0 0x3000000 ${kernel_image};" \
"fatload mmc 0 0x2A00000 ${devicetree_image};" \
"bootm 0x3000000 - 0x2A00000\0" \






// commands to compile u-boot
export CROSS_COMPILE=arm-xilinx-linux-gnueabi-
export ARCH=arm
make zynq_zed_config
make

export CROSS_COMPILE=arm-xilinx-linux-gnueabi-
make ARCH=arm xilinx_zynq_defconfig

obj-$(CONFIG_MXP) += mxp.o
obj-$(CONFIG_CM_ALLOCATOR) += cma.o

CONFIG_MXP=m
CONFIG_CM_ALLOCATOR=y

make ARCH=arm UIMAGE_LOADADDR=0x8000 uImage
make ARCH=arm modules




make ARCH=arm modules_install INSTALL_MOD_PATH=/path/to/rootfs(
ext4partition)/in/sdcard/





// Generating the dtb
./scripts/dtc/dtc -I dts -O dtb arch/arm/boot/dts/zynq-zed.dts -o mxp.dtb
./scripts/dtc/dtc -I dtb -O dts mxp.dtb -o mxp_linux.dts





//change this content
bootargs = "console=ttyPS0,115200 root=/dev/ram rw earlyprintk";
//to this content
bootargs = "console=ttyPS0,115200 root=/dev/mmcblk0p2 rw earlyprintk
rootfstype=ext4 rootwait devtmpfs.mount=0";




// hsi command opens up the TCL shell 
hsi
hsi% open_hw_design system.hdf
hsi% set_repo_path /path/to/Device tree repo/
hsi% create_sw_design mxp_device_tree -os device_tree -proc
ps7_cortexa9_0
hsi% generate_target -dir mxp_dts
hsi% quit





// DTS entry for MXP

 amba_pl: amba_pl {

 #address-cells = <1>;

 #size-cells = <1>;

 compatible = "simple-bus";

 ranges ;

 vectorblox_mxp_arm_0: vectorblox_mxp@b0000000 {

 compatible = "xlnx,vectorblox-mxp-1.0";

 reg = <0xb0000000 0x10000 0x40000000 0x100000>;

 vblx, = <1>;

 vblx,archical = <0>;

 vblx,beats_per_burst = <16>;

 vblx,burstlength_bytes = <128>;

 vblx,c_instr_port_type = <2>;

 vblx,c_m_axi_addr_width = <32>;

 vblx,c_m_axi_data_width = <64>;

 vblx,c_m_axi_len_width = <4>;

 vblx,c_m_axi_supports_threads = <0>;

 vblx,c_s_axi_addr_width = <32>;

 vblx,c_s_axi_baseaddr = <0xB0000000>;

 vblx,c_s_axi_data_width = <32>;

 vblx,c_s_axi_highaddr = <0xB000FFFF>;

 vblx,c_s_axi_instr_addr_width = <32>;

 vblx,c_s_axi_instr_baseaddr = <0x40000000>;

 vblx,c_s_axi_instr_data_width = <32>;

 vblx,c_s_axi_instr_highaddr = <0x400FFFFF>;

 vblx,c_s_axi_instr_id_width = <6>;

 .........................................

 .........................................

 .........................................

 vblx,vector_lanes = <16>;

 };

};





compatible = "vectorblox.com,vectorblox-mxp-1.0";






vectorblox_mxp@40000000{
reg = <0x40000000 0x100000 0xb0000000 0x10000>;



./scripts/dtc/dtc -I dts -O dtb mxp_linux.dts -o devicetree.dtb





 /* Preparing the BOOT.bin package.
    Make sure the contents are ordered 
    as shown
 */  
the_ROM_image:
{
 [bootloader] <path to fsbl.elf>
 <path to bitstream file>
 <path to u-boot.elf>
}
 
 
 
 
 
 
 
 bootgen -image bootimage.bif -o i boot.bin -w on
 
 
 
 
 ............
 ............
 ............
 mxp_init
 mxp_probe
 ............
 ............
 ............
 
 
 
 
// configuration parameters for the MXP setup
vector_lanes = 16
core_freq = 100.0e6
scratchpad_size = 65536
dma data width in bytes = 8






 //MXP application for Image negation
 int main(int argc, char *argv[])
 {
 pgm_t opgm;
 pgm_t ipgm;
 int img_size = 0;
 unsigned char *v_sub = NULL;
 unsigned char max_val = 255;
 // MXP initialization
 VectorBlox_MXP_Initialize("mxp0","cma"); 
 //Reading input image
 readPGM(&ipgm,"input_lena.pgm");  
 img_size = (ipgm.width * ipgm.height);
  //Allocate buffer for output image
 opgm.width = ipgm.width; 
 opgm.height = ipgm.height;
 opgm.buf = (unsigned char*)vbx_shared_malloc(img_size * sizeof(unsigned char));
  //Allocate vector on scratchpad
 v_sub = (unsigned char *)vbx_sp_malloc(img_size * sizeof(unsigned char )); 
 //Transfer input bytes from memory to scratchpad
 vbx_dma_to_vector(v_sub, ipgm.buf, img_size);
 vbx_set_vl(img_size);
 //Scalar vector subtraction for taking negative of image
 vbx(SVBU, VSUB, v_sub, max_val, v_sub);
 //Writing result from scratchpad to memory
 vbx_dma_to_host(opgm.buf, v_sub, img_size);
 vbx_sync();
 //Writing output image
 writePGM(&opgm,"out_lena_negative.pgm");
 //Free allocated pointers
 vbx_sp_free();
 vbx_shared_free(ipgm.buf);
 vbx_shared_free(opgm.buf);
 }




















