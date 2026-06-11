#define vsetvl64(v){\
	long vl;\
	asm volatile("vsetvli %0, %1, e64, m1\n" : "+r"(vl) : "r"(v));\
}

#define vsetvl32(v){\
	long vl;\
	asm volatile("vsetvli %0, %1, e32, m1\n" : "+r"(vl) : "r"(v));\
}

long getvlmax64(){
	long vl;
	asm volatile("vsetvli %0, x0, e64, m1\n" : "+r"(vl));
	return vl;
}

#include "rave_user_events.h"

#define imm 5
int main(){
	double dpscalar = 4.0;
	float fpscalar = 5.2f;
	int scalar32 = 6;
	long scalar64 = 3;
	long scalar64_2 = 5;
	long array[1024];
	long stride=2;
	long vlmax=getvlmax64();
	long vlhalf = vlmax/2;

	//Integer arith
	rave_begin_region("Integer_arith");
	vsetvl64(vlmax);
	asm volatile("vaadd.vi v8, v24, %0\n" ::"i"(imm): "v8" , "v16" , "v24");
	asm volatile("vaadd.vv v8, v24, v16\n" ::: "v8" , "v16" , "v24");
	asm volatile("vaadd.vx v8, v24, %0\n" :"+r"(scalar64):: "v8" , "v16" , "v24");
	asm volatile("vadc.vim v8, v24, %0, v0\n" ::"i"(imm): "v8" , "v16" , "v24");
	asm volatile("vadc.vvm v4, v4, v8, v0\n" ::: "v8" , "v16" , "v24");
	asm volatile("vadc.vxm v8, v24, %0, v0\n" :"+r"(scalar64):: "v8" , "v16" , "v24");
	asm volatile("vadd.vi v8, v24, %0\n" ::"i"(imm): "v8" , "v16" , "v24");
	asm volatile("vadd.vv v8, v24, v16\n" ::: "v8" , "v16" , "v24");
	asm volatile("vadd.vx v8, v24, %0\n" :"+r"(scalar64):: "v8" , "v16" , "v24");
	asm volatile("vand.vi v8, v24, %0\n" ::"i"(imm): "v8" , "v16" , "v24");
	asm volatile("vand.vv v8, v24, v16\n" ::: "v8" , "v16" , "v24");
	asm volatile("vand.vx v8, v24, %0\n" :"+r"(scalar64):: "v8" , "v16" , "v24");
	asm volatile("vasub.vv v8, v24, v16\n" ::: "v8" , "v16" , "v24");
	asm volatile("vasub.vx v8, v24, %0\n" :"+r"(scalar64):: "v8" , "v16" , "v24");
	asm volatile("vdivu.vv v8, v24, v16\n" ::: "v8" , "v16" , "v24");
	asm volatile("vdivu.vx v8, v24, %0\n" :"+r"(scalar64):: "v8" , "v16" , "v24");
	asm volatile("vdiv.vv v8, v24, v16\n" ::: "v8" , "v16" , "v24");
	asm volatile("vdiv.vx v8, v24, %0\n" :"+r"(scalar64):: "v8" , "v16" , "v24");
	asm volatile("vmacc.vv v8, v16, v24\n" ::: "v8" , "v16" , "v24");
	asm volatile("vmacc.vx v8, %0, v24\n" :"+r"(scalar64):: "v8" , "v16" , "v24");
	asm volatile("vmadc.vim v8, v24, %0, v0\n" ::"i"(imm): "v8" , "v16" , "v24");
	asm volatile("vmadc.vvm v1, v4, v8, v0\n" ::: "v8" , "v16" , "v24");
	asm volatile("vmadc.vxm v8, v24, %0, v0\n" :"+r"(scalar64):: "v8" , "v16" , "v24");
	asm volatile("vmadd.vv v8, v16, v24\n" ::: "v8" , "v16" , "v24");
	asm volatile("vmadd.vx v8, %0, v24\n" :"+r"(scalar64):: "v8" , "v16" , "v24");
	asm volatile("vmaxu.vv v8, v24, v16\n" ::: "v8" , "v16" , "v24");
	asm volatile("vmaxu.vx v8, v24, %0\n" :"+r"(scalar64):: "v8" , "v16" , "v24");
	asm volatile("vmax.vv v8, v24, v16\n" ::: "v8" , "v16" , "v24");
	asm volatile("vmax.vx v8, v24, %0\n" :"+r"(scalar64):: "v8" , "v16" , "v24");
	asm volatile("vminu.vv v8, v24, v16\n" ::: "v8" , "v16" , "v24");
	asm volatile("vminu.vx v8, v24, %0\n" :"+r"(scalar64):: "v8" , "v16" , "v24");
	asm volatile("vmin.vv v8, v24, v16\n" ::: "v8" , "v16" , "v24");
	asm volatile("vmin.vx v8, v24, %0\n" :"+r"(scalar64):: "v8" , "v16" , "v24");
	asm volatile("vmulhsu.vv v8, v24, v16\n" ::: "v8" , "v16" , "v24");
	asm volatile("vmulhsu.vx v8, v24, %0\n" :"+r"(scalar64):: "v8" , "v16" , "v24");
	asm volatile("vmulhu.vv v8, v24, v16\n" ::: "v8" , "v16" , "v24");
	asm volatile("vmulhu.vx v8, v24, %0\n" :"+r"(scalar64):: "v8" , "v16" , "v24");
	asm volatile("vmulh.vv v8, v24, v16\n" ::: "v8" , "v16" , "v24");
	asm volatile("vmulh.vx v8, v24, %0\n" :"+r"(scalar64):: "v8" , "v16" , "v24");
	asm volatile("vmul.vv v8, v24, v16\n" ::: "v8" , "v16" , "v24");
	asm volatile("vmul.vx v8, v24, %0\n" :"+r"(scalar64):: "v8" , "v16" , "v24");
	asm volatile("vor.vi v8, v24, %0\n" ::"i"(imm): "v8" , "v16" , "v24");
	asm volatile("vor.vv v8, v24, v16\n" ::: "v8" , "v16" , "v24");
	asm volatile("vor.vx v8, v24, %0\n" :"+r"(scalar64):: "v8" , "v16" , "v24");
	asm volatile("vremu.vv v8, v24, v16\n" ::: "v8" , "v16" , "v24");
	asm volatile("vremu.vx v8, v24, %0\n" :"+r"(scalar64):: "v8" , "v16" , "v24");
	asm volatile("vrem.vv v8, v24, v16\n" ::: "v8" , "v16" , "v24");
	asm volatile("vrem.vx v8, v24, %0\n" :"+r"(scalar64):: "v8" , "v16" , "v24");
	asm volatile("vrsub.vi v8, v24, %0\n" ::"i"(imm): "v8" , "v16" , "v24");
	asm volatile("vrsub.vx v8, v24, %0\n" :"+r"(scalar64):: "v8" , "v16" , "v24");
	asm volatile("vsaddu.vi v8, v24, %0\n" ::"i"(imm): "v8" , "v16" , "v24");
	asm volatile("vsaddu.vv v8, v24, v16\n" ::: "v8" , "v16" , "v24");
	asm volatile("vsaddu.vx v8, v24, %0\n" :"+r"(scalar64):: "v8" , "v16" , "v24");
	asm volatile("vsadd.vi v8, v24, %0\n" ::"i"(imm): "v8" , "v16" , "v24");
	asm volatile("vsadd.vv v8, v24, v16\n" ::: "v8" , "v16" , "v24");
	asm volatile("vsadd.vx v8, v24, %0\n" :"+r"(scalar64):: "v8" , "v16" , "v24");
	asm volatile("vsbc.vvm v8, v24, v16, v0\n" ::: "v8" , "v16" , "v24");
	asm volatile("vsbc.vxm v8, v24, %0, v0\n" :"+r"(scalar64):: "v8" , "v16" , "v24");
	asm volatile("vsll.vi v8, v24, %0\n" ::"i"(imm): "v8" , "v16" , "v24");
	asm volatile("vsll.vv v8, v24, v16\n" ::: "v8" , "v16" , "v24");
	asm volatile("vsll.vx v8, v24, %0\n" :"+r"(scalar64):: "v8" , "v16" , "v24");
	asm volatile("vsmul.vv v8, v24, v16\n" ::: "v8" , "v16" , "v24");
	asm volatile("vsmul.vx v8, v24, %0\n" :"+r"(scalar64):: "v8" , "v16" , "v24");
	asm volatile("vsra.vi v8, v24, %0\n" ::"i"(imm): "v8" , "v16" , "v24");
	asm volatile("vsra.vv v8, v24, v16\n" ::: "v8" , "v16" , "v24");
	asm volatile("vsra.vx v8, v24, %0\n" :"+r"(scalar64):: "v8" , "v16" , "v24");
	asm volatile("vsrl.vi v8, v24, %0\n" ::"i"(imm): "v8" , "v16" , "v24");
	asm volatile("vsrl.vv v8, v24, v16\n" ::: "v8" , "v16" , "v24");
	asm volatile("vsrl.vx v8, v24, %0\n" :"+r"(scalar64):: "v8" , "v16" , "v24");
	asm volatile("vssra.vi v8, v24, %0\n" ::"i"(imm): "v8" , "v16" , "v24");
	asm volatile("vssra.vv v8, v24, v16\n" ::: "v8" , "v16" , "v24");
	asm volatile("vssra.vx v8, v24, %0\n" :"+r"(scalar64):: "v8" , "v16" , "v24");
	asm volatile("vssrl.vi v8, v24, %0\n" ::"i"(imm): "v8" , "v16" , "v24");
	asm volatile("vssrl.vv v8, v24, v16\n" ::: "v8" , "v16" , "v24");
	asm volatile("vssrl.vx v8, v24, %0\n" :"+r"(scalar64):: "v8" , "v16" , "v24");
	asm volatile("vssubu.vv v8, v24, v16\n" ::: "v8" , "v16" , "v24");
	asm volatile("vssubu.vx v8, v24, %0\n" :"+r"(scalar64):: "v8" , "v16" , "v24");
	asm volatile("vssub.vv v8, v24, v16\n" ::: "v8" , "v16" , "v24");
	asm volatile("vssub.vx v8, v24, %0\n" :"+r"(scalar64):: "v8" , "v16" , "v24");
	asm volatile("vsub.vv v8, v24, v16\n" ::: "v8" , "v16" , "v24");
	asm volatile("vsub.vx v8, v24, %0\n" :"+r"(scalar64):: "v8" , "v16" , "v24");
	asm volatile("vxor.vi v8, v24, %0\n" ::"i"(imm): "v8" , "v16" , "v24");
	asm volatile("vxor.vv v8, v24, v16\n" ::: "v8" , "v16" , "v24");
	asm volatile("vxor.vx v8, v24, %0\n" :"+r"(scalar64):: "v8" , "v16" , "v24");
	asm volatile("vmsbc.vvm v8, v24, v16, v0\n" ::: "v8" , "v16" , "v24");
	asm volatile("vmsbc.vxm v8, v24, %0, v0\n" :"+r"(scalar64):: "v8" , "v16" , "v24");
	//Int Widenings
	vsetvl32(vlhalf);
	asm volatile("vwaddu.vv v8, v24, v16\n" ::: "v8" , "v16" , "v24"); 
	asm volatile("vwaddu.wv v8, v24, v16\n" ::: "v8" , "v16" , "v24");
	asm volatile("vwadd.vv v8, v24, v16\n" ::: "v8" , "v16" , "v24");
	asm volatile("vwadd.wv v8, v24, v16\n" ::: "v8" , "v16" , "v24");
	asm volatile("vwmaccsu.vv v8, v16, v24\n" ::: "v8" , "v16" , "v24");
	asm volatile("vwmaccu.vv v8, v16, v24\n" ::: "v8" , "v16" , "v24");
	asm volatile("vwmacc.vv v8, v16, v24\n" ::: "v8" , "v16" , "v24");
	asm volatile("vwmulsu.vv v8, v24, v16\n" ::: "v8" , "v16" , "v24");


	asm volatile("vwmulu.vv v8, v24, v16\n" ::: "v8" , "v16" , "v24");
	asm volatile("vwmul.vv v8, v24, v16\n" ::: "v8" , "v16" , "v24");
	asm volatile("vwsubu.vv v8, v24, v16\n" ::: "v8" , "v16" , "v24");
	asm volatile("vwsubu.wv v8, v24, v16\n" ::: "v8" , "v16" , "v24");
	asm volatile("vwsub.vv v8, v24, v16\n" ::: "v8" , "v16" , "v24");
	asm volatile("vwsub.wv v8, v24, v16\n" ::: "v8" , "v16" , "v24");
	asm volatile("vwaddu.vx v8, v24, %0\n" :"+r"(scalar32):: "v8" , "v16" , "v24");
	asm volatile("vwaddu.wx v8, v24, %0\n" :"+r"(scalar32):: "v8" , "v16" , "v24");
	
	asm volatile("vwadd.vx v8, v24, %0\n" :"+r"(scalar32):: "v8" , "v16" , "v24");
	asm volatile("vwadd.wx v8, v24, %0\n" :"+r"(scalar32):: "v8" , "v16" , "v24");
	asm volatile("vwmaccsu.vx v8, %0, v24\n" :"+r"(scalar32):: "v8" , "v16" , "v24");
	asm volatile("vwmaccus.vx v8, %0, v24\n" :"+r"(scalar32):: "v8" , "v16" , "v24");
	
	asm volatile("vwmaccu.vx v8, %0, v24\n" :"+r"(scalar32):: "v8" , "v16" , "v24");
	asm volatile("vwmacc.vx v8, %0, v24\n" :"+r"(scalar32):: "v8" , "v16" , "v24");
	
	asm volatile("vwmulsu.vx v8, v24, %0\n" :"+r"(scalar32):: "v8" , "v16" , "v24");
	asm volatile("vwmulu.vx v8, v24, %0\n" :"+r"(scalar32):: "v8" , "v16" , "v24");
	asm volatile("vwmul.vx v8, v24, %0\n" :"+r"(scalar32):: "v8" , "v16" , "v24");
	asm volatile("vwsubu.vx v8, v24, %0\n" :"+r"(scalar32):: "v8" , "v16" , "v24");
	asm volatile("vwsubu.wx v8, v24, %0\n" :"+r"(scalar32):: "v8" , "v16" , "v24");
	asm volatile("vwsub.vx v8, v24, %0\n" :"+r"(scalar32):: "v8" , "v16" , "v24");
	asm volatile("vwsub.wx v8, v24, %0\n" :"+r"(scalar32):: "v8" , "v16" , "v24");

	asm volatile("vwsmaccsu.vv v8, v16, v24\n" ::: "v8" , "v16" , "v24");
	asm volatile("vwsmaccsu.vx v8, %0, v24\n" :"+r"(scalar32):: "v8" , "v16" , "v24");
	asm volatile("vwsmaccus.vx v8, %0, v24\n" :"+r"(scalar32):: "v8" , "v16" , "v24");
	asm volatile("vwsmaccu.vv v8, v16, v24\n" ::: "v8" , "v16" , "v24");
	asm volatile("vwsmaccu.vx v8, %0, v24\n" :"+r"(scalar32):: "v8" , "v16" , "v24");
	asm volatile("vwsmacc.vv v8, v16, v24\n" ::: "v8" , "v16" , "v24");
	asm volatile("vwsmacc.vx v8, %0, v24\n" :"+r"(scalar32):: "v8" , "v16" , "v24");
	

	//INT narrowings
	asm volatile("vnmsac.vv v8, v16, v24\n" ::: "v8" , "v16" , "v24");
	asm volatile("vnmsub.vv v8, v16, v24\n" ::: "v8" , "v16" , "v24");
	asm volatile("vnsra.vv v8, v24, v16\n" ::: "v8" , "v16" , "v24");
	asm volatile("vnsrl.vv v8, v24, v16\n" ::: "v8" , "v16" , "v24");

	asm volatile("vnmsac.vx v8, %0, v24\n" :"+r"(scalar32):: "v8" , "v16" , "v24");
	asm volatile("vnmsub.vx v8, %0, v24\n" :"+r"(scalar32):: "v8" , "v16" , "v24");
	asm volatile("vnsra.vi v8, v24, %0\n" ::"i"(imm): "v8" , "v16" , "v24");
	asm volatile("vnsra.vx v8, v24, %0\n" :"+r"(scalar32):: "v8" , "v16" , "v24");
	asm volatile("vnsrl.vi v8, v24, %0\n" ::"i"(imm): "v8" , "v16" , "v24");
	asm volatile("vnsrl.vx v8, v24, %0\n" :"+r"(scalar32):: "v8" , "v16" , "v24");
	rave_end_region("Integer_arith");

	//Integer reduction
	rave_begin_region("Integer_reduction");
	vsetvl64(vlmax);
	asm volatile("vredand.vs v8, v24, v16\n" ::: "v8" , "v16" , "v24");
	asm volatile("vredmaxu.vs v8, v24, v16\n" ::: "v8" , "v16" , "v24");
	asm volatile("vredmax.vs v8, v24, v16\n" ::: "v8" , "v16" , "v24");
	asm volatile("vredminu.vs v8, v24, v16\n" ::: "v8" , "v16" , "v24");
	asm volatile("vredmin.vs v8, v24, v16\n" ::: "v8" , "v16" , "v24");
	asm volatile("vredor.vs v8, v24, v16\n" ::: "v8" , "v16" , "v24");
	asm volatile("vredsum.vs v8, v24, v16\n" ::: "v8" , "v16" , "v24");
	asm volatile("vredxor.vs v8, v24, v16\n" ::: "v8" , "v16" , "v24");

	vsetvl32(vlhalf);
	asm volatile("vwredsumu.vs v8, v24, v16\n" ::: "v8" , "v16" , "v24");
	asm volatile("vwredsum.vs v8, v24, v16\n" ::: "v8" , "v16" , "v24");
	rave_end_region("Integer_reduction");

	//Other
	rave_begin_region("Other");
	vsetvl64(vlmax);
	asm volatile("vcompress.vm v8, v24, v16\n" ::: "v8" , "v16" , "v24");
	asm volatile("vext.x.v %0, v24, %1\n" :"+r"(scalar64) , "+r"(scalar64_2):: "v8" , "v16" , "v24");
	asm volatile("vid.v v8\n" ::: "v8" , "v16" , "v24");
	asm volatile("viota.m v8, v24\n" ::: "v8" , "v16" , "v24");
	asm volatile("vmerge.vim v8, v24, %0, v0\n" ::"i"(imm): "v8" , "v16" , "v24");
	asm volatile("vmerge.vvm v8, v24, v16, v0\n" ::: "v8" , "v16" , "v24");
	asm volatile("vmerge.vxm v8, v24, %0, v0\n" :"+r"(scalar64):: "v8" , "v16" , "v24");
	asm volatile("vmfirst.m %0, v24\n" :"+r"(scalar64):: "v8" , "v16" , "v24");
	asm volatile("vmpopc.m %0, v24\n" :"+r"(scalar64):: "v8" , "v16" , "v24");
	asm volatile("vmv.s.x v8, %0\n" :"+r"(scalar64):: "v8" , "v16" , "v24");
	asm volatile("vmv.v.i v24, %0\n" ::"i"(imm): "v8" , "v16" , "v24"); //writes to v24, for indexes!
	asm volatile("vmv.v.v v8, v16\n" ::: "v8" , "v16" , "v24");
	asm volatile("vmv.v.x v8, %0\n" :"+r"(scalar64):: "v8" , "v16" , "v24");
	asm volatile("vslide1down.vx v8, v24, %0\n" :"+r"(scalar64):: "v8" , "v16" , "v24");
	asm volatile("vslide1up.vx v8, v24, %0\n" :"+r"(scalar64):: "v8" , "v16" , "v24");
	asm volatile("vrgather.vi v8, v24, %0\n" ::"i"(imm): "v8" , "v16" , "v24");
	asm volatile("vrgather.vv v8, v24, v16\n" ::: "v8" , "v16" , "v24");
	asm volatile("vrgather.vx v8, v24, %0\n" :"+r"(scalar64):: "v8" , "v16" , "v24");
	asm volatile("vslidedown.vi v8, v24, %0\n" ::"i"(imm): "v8" , "v16" , "v24");
	asm volatile("vslidedown.vx v8, v24, %0\n" :"+r"(scalar64):: "v8" , "v16" , "v24");
	asm volatile("vslideup.vi v8, v24, %0\n" ::"i"(imm): "v8" , "v16" , "v24");
	asm volatile("vslideup.vx v8, v24, %0\n" :"+r"(scalar64):: "v8" , "v16" , "v24");
	asm volatile("vfclass.v v8, v24\n" ::: "v8" , "v16" , "v24");
	asm volatile("vfcvt.f.xu.v v8, v24\n" ::: "v8" , "v16" , "v24");
	asm volatile("vfcvt.f.x.v v8, v24\n" ::: "v8" , "v16" , "v24");
	asm volatile("vfcvt.x.f.v v8, v24\n" ::: "v8" , "v16" , "v24");
	asm volatile("vfcvt.xu.f.v v8, v24\n" ::: "v8" , "v16" , "v24");
	asm volatile("vfmerge.vfm v8, v24, %0, v0\n" :"+f"(dpscalar):: "v8" , "v16" , "v24");
	asm volatile("vfmv.f.s %0, v24\n" :"+f"(dpscalar):: "v8" , "v16" , "v24");
	asm volatile("vfmv.s.f v8, %0\n" :"+f"(dpscalar):: "v8" , "v16" , "v24");
	asm volatile("vfmv.v.f v8, %0\n" :"+f"(dpscalar):: "v8" , "v16" , "v24");
	vsetvl32(vlhalf)
	asm volatile("vfncvt.f.f.v v8, v24\n" ::: "v8" , "v16" , "v24");
	asm volatile("vfncvt.f.xu.v v8, v24\n" ::: "v8" , "v16" , "v24");
	asm volatile("vfncvt.f.x.v v8, v24\n" ::: "v8" , "v16" , "v24");
	asm volatile("vfncvt.x.f.v v8, v24\n" ::: "v8" , "v16" , "v24");
	asm volatile("vfncvt.xu.f.v v8, v24\n" ::: "v8" , "v16" , "v24");
	asm volatile("vfwcvt.f.f.v v8, v24\n" ::: "v8" , "v16" , "v24");
	asm volatile("vfwcvt.f.xu.v v8, v24\n" ::: "v8" , "v16" , "v24");
	asm volatile("vfwcvt.f.x.v v8, v24\n" ::: "v8" , "v16" , "v24");
	asm volatile("vfwcvt.x.f.v v8, v24\n" ::: "v8" , "v16" , "v24");
	asm volatile("vfwcvt.xu.f.v v8, v24\n" ::: "v8" , "v16" , "v24");
	asm volatile("vnclipu.vi v8, v24, %0\n" ::"i"(imm): "v8" , "v16" , "v24");
	asm volatile("vnclipu.vx v8, v24, %0\n" :"+r"(scalar32):: "v8" , "v16" , "v24");
	asm volatile("vnclip.vi v8, v24, %0\n" ::"i"(imm): "v8" , "v16" , "v24");
	asm volatile("vnclip.vx v8, v24, %0\n" :"+r"(scalar32):: "v8" , "v16" , "v24");
	asm volatile("vnclipu.vv v8, v24, v16\n" ::: "v8" , "v16" , "v24");
	asm volatile("vnclip.vv v8, v24, v16\n" ::: "v8" , "v16" , "v24");
	
	rave_end_region("Other");

	//FP Arith
	rave_begin_region("FP_arith");
	vsetvl64(vlmax);
	asm volatile("vfadd.vf v8, v24, %0\n" :"+f"(dpscalar):: "v8" , "v16" , "v24");
	asm volatile("vfadd.vv v8, v24, v16\n" ::: "v8" , "v16" , "v24");
	asm volatile("vfdiv.vf v8, v24, %0\n" :"+f"(dpscalar):: "v8" , "v16" , "v24");
	asm volatile("vfdiv.vv v8, v24, v16\n" ::: "v8" , "v16" , "v24");
	asm volatile("vfmacc.vf v8, %0, v24\n" :"+f"(dpscalar):: "v8" , "v16" , "v24");
	asm volatile("vfmacc.vv v8, v16, v24\n" ::: "v8" , "v16" , "v24");
	asm volatile("vfmadd.vf v8, %0, v24\n" :"+f"(dpscalar):: "v8" , "v16" , "v24");
	asm volatile("vfmadd.vv v8, v16, v24\n" ::: "v8" , "v16" , "v24");
	asm volatile("vfmax.vf v8, v24, %0\n" :"+f"(dpscalar):: "v8" , "v16" , "v24");
	asm volatile("vfmax.vv v8, v24, v16\n" ::: "v8" , "v16" , "v24");
	asm volatile("vfmin.vf v8, v24, %0\n" :"+f"(dpscalar):: "v8" , "v16" , "v24");
	asm volatile("vfmin.vv v8, v24, v16\n" ::: "v8" , "v16" , "v24");
	asm volatile("vfmsac.vf v8, %0, v24\n" :"+f"(dpscalar):: "v8" , "v16" , "v24");
	asm volatile("vfmsac.vv v8, v16, v24\n" ::: "v8" , "v16" , "v24");
	asm volatile("vfmsub.vf v8, %0, v24\n" :"+f"(dpscalar):: "v8" , "v16" , "v24");
	asm volatile("vfmsub.vv v8, v16, v24\n" ::: "v8" , "v16" , "v24");
	asm volatile("vfmul.vf v8, v24, %0\n" :"+f"(dpscalar):: "v8" , "v16" , "v24");
	asm volatile("vfmul.vv v8, v24, v16\n" ::: "v8" , "v16" , "v24");
	asm volatile("vfrdiv.vf v8, v24, %0\n" :"+f"(dpscalar):: "v8" , "v16" , "v24");
	asm volatile("vfrsub.vf v8, v24, %0\n" :"+f"(dpscalar):: "v8" , "v16" , "v24");
	asm volatile("vfsgnjn.vf v8, v24, %0\n" :"+f"(dpscalar):: "v8" , "v16" , "v24");
	asm volatile("vfsgnjn.vv v8, v24, v16\n" ::: "v8" , "v16" , "v24");
	asm volatile("vfsgnj.vf v8, v24, %0\n" :"+f"(dpscalar):: "v8" , "v16" , "v24");
	asm volatile("vfsgnj.vv v8, v24, v16\n" ::: "v8" , "v16" , "v24");
	asm volatile("vfsgnjx.vf v8, v24, %0\n" :"+f"(dpscalar):: "v8" , "v16" , "v24");
	asm volatile("vfsgnjx.vv v8, v24, v16\n" ::: "v8" , "v16" , "v24");
	asm volatile("vfsqrt.v v8, v24\n" ::: "v8" , "v16" , "v24");
	asm volatile("vfsub.vf v8, v24, %0\n" :"+f"(dpscalar):: "v8" , "v16" , "v24");
	asm volatile("vfsub.vv v8, v24, v16\n" ::: "v8" , "v16" , "v24");

	vsetvl32(vlhalf)
		//FP narrowings
	asm volatile("vfnmacc.vv v8, v16, v24\n" ::: "v8" , "v16" , "v24");
	asm volatile("vfnmadd.vv v8, v16, v24\n" ::: "v8" , "v16" , "v24");
	asm volatile("vfnmsac.vv v8, v16, v24\n" ::: "v8" , "v16" , "v24");
	asm volatile("vfnmsub.vv v8, v16, v24\n" ::: "v8" , "v16" , "v24");

	asm volatile("vfnmacc.vf v8, %0, v24\n" :"+f"(fpscalar):: "v8" , "v16" , "v24");
	asm volatile("vfnmadd.vf v8, %0, v24\n" :"+f"(fpscalar):: "v8" , "v16" , "v24");
	asm volatile("vfnmsac.vf v8, %0, v24\n" :"+f"(fpscalar):: "v8" , "v16" , "v24");
	asm volatile("vfnmsub.vf v8, %0, v24\n" :"+f"(fpscalar):: "v8" , "v16" , "v24");

	//FP Widenings
	asm volatile("vfwadd.vv v8, v24, v16\n" ::: "v8" , "v16" , "v24");
	asm volatile("vfwadd.wv v8, v24, v16\n" ::: "v8" , "v16" , "v24");

	asm volatile("vfwmacc.vv v8, v16, v24\n" ::: "v8" , "v16" , "v24");
	asm volatile("vfwmsac.vv v8, v16, v24\n" ::: "v8" , "v16" , "v24");
	asm volatile("vfwmul.vv v8, v24, v16\n" ::: "v8" , "v16" , "v24");
	asm volatile("vfwnmacc.vv v8, v16, v24\n" ::: "v8" , "v16" , "v24");
	asm volatile("vfwnmsac.vv v8, v16, v24\n" ::: "v8" , "v16" , "v24");
	asm volatile("vfwsub.vv v8, v24, v16\n" ::: "v8" , "v16" , "v24");
	asm volatile("vfwsub.wv v8, v24, v16\n" ::: "v8" , "v16" , "v24");

	asm volatile("vfwadd.vf v8, v24, %0\n" :"+f"(fpscalar):: "v8" , "v16" , "v24");
	asm volatile("vfwadd.wf v8, v24, %0\n" :"+f"(fpscalar):: "v8" , "v16" , "v24");
	asm volatile("vfwmacc.vf v8, %0, v24\n" :"+f"(fpscalar):: "v8" , "v16" , "v24");
	asm volatile("vfwmsac.vf v8, %0, v24\n" :"+f"(fpscalar):: "v8" , "v16" , "v24");
	asm volatile("vfwmul.vf v8, v24, %0\n" :"+f"(fpscalar):: "v8" , "v16" , "v24");
	asm volatile("vfwnmacc.vf v8, %0, v24\n" :"+f"(fpscalar):: "v8" , "v16" , "v24");
	asm volatile("vfwnmsac.vf v8, %0, v24\n" :"+f"(fpscalar):: "v8" , "v16" , "v24");
	asm volatile("vfwsub.vf v8, v24, %0\n" :"+f"(fpscalar):: "v8" , "v16" , "v24");
	asm volatile("vfwsub.wf v8, v24, %0\n" :"+f"(fpscalar):: "v8" , "v16" , "v24");
	rave_end_region("FP_arith");

	rave_begin_region("FP_reductions");
	asm volatile("vfredmax.vs v8, v24, v16\n" ::: "v8" , "v16" , "v24");
	asm volatile("vfredmin.vs v8, v24, v16\n" ::: "v8" , "v16" , "v24");
	asm volatile("vfredosum.vs v8, v24, v16\n" ::: "v8" , "v16" , "v24");
	asm volatile("vfredsum.vs v8, v24, v16\n" ::: "v8" , "v16" , "v24");
	asm volatile("vfwredosum.vs v8, v24, v16\n" ::: "v8" , "v16" , "v24");
	asm volatile("vfwredsum.vs v8, v24, v16\n" ::: "v8" , "v16" , "v24");
	rave_end_region("FP_reductions");

	//Mask
	rave_begin_region("Mask creation");
	vsetvl64(vlmax);
	asm volatile("vmford.vf v8, v24, %0\n" :"+f"(dpscalar):: "v8" , "v16" , "v24");
	asm volatile("vmford.vv v8, v24, v16\n" ::: "v8" , "v16" , "v24");
	asm volatile("vmsbf.m v8, v24\n" ::: "v8" , "v16" , "v24");
	asm volatile("vmsof.m v8, v24\n" ::: "v8" , "v16" , "v24");
	asm volatile("vmand.mm v8, v24, v16\n" ::: "v8" , "v16" , "v24");
	asm volatile("vmandnot.mm v8, v24, v16\n" ::: "v8" , "v16" , "v24");
	asm volatile("vmclr.m v8\n" ::: "v8" , "v16" , "v24");
	asm volatile("vmcpy.m v8, v16\n" ::: "v8" , "v16" , "v24");
	asm volatile("vmfeq.vf v8, v24, %0\n" :"+f"(dpscalar):: "v8" , "v16" , "v24");
	asm volatile("vmfeq.vv v8, v24, v16\n" ::: "v8" , "v16" , "v24");
	asm volatile("vmfge.vf v8, v24, %0\n" :"+f"(dpscalar):: "v8" , "v16" , "v24");
	asm volatile("vmfge.vv v8, v16, v24\n" ::: "v8" , "v16" , "v24");
	asm volatile("vmfgt.vf v8, v24, %0\n" :"+f"(dpscalar):: "v8" , "v16" , "v24");
	asm volatile("vmfgt.vv v8, v16, v24\n" ::: "v8" , "v16" , "v24");
	asm volatile("vmfle.vf v8, v24, %0\n" :"+f"(dpscalar):: "v8" , "v16" , "v24");
	asm volatile("vmfle.vv v8, v24, v16\n" ::: "v8" , "v16" , "v24");
	asm volatile("vmflt.vf v8, v24, %0\n" :"+f"(dpscalar):: "v8" , "v16" , "v24");
	asm volatile("vmflt.vv v8, v24, v16\n" ::: "v8" , "v16" , "v24");
	asm volatile("vmfne.vf v8, v24, %0\n" :"+f"(dpscalar):: "v8" , "v16" , "v24");
	asm volatile("vmfne.vv v8, v24, v16\n" ::: "v8" , "v16" , "v24");
	asm volatile("vmnand.mm v8, v24, v16\n" ::: "v8" , "v16" , "v24");
	asm volatile("vmnor.mm v8, v24, v16\n" ::: "v8" , "v16" , "v24");
	asm volatile("vmnot.m v8, v16\n" ::: "v8" , "v16" , "v24");
	asm volatile("vmor.mm v8, v24, v16\n" ::: "v8" , "v16" , "v24");
	asm volatile("vmornot.mm v8, v24, v16\n" ::: "v8" , "v16" , "v24");
	asm volatile("vmseq.vi v8, v24, %0\n" ::"i"(imm): "v8" , "v16" , "v24");
	asm volatile("vmseq.vv v8, v24, v16\n" ::: "v8" , "v16" , "v24");
	asm volatile("vmseq.vx v8, v24, %0\n" :"+r"(scalar64):: "v8" , "v16" , "v24");
	asm volatile("vmset.m v8\n" ::: "v8" , "v16" , "v24");
	asm volatile("vmsgtu.vi v8, v24, %0\n" ::"i"(imm): "v8" , "v16" , "v24");
	asm volatile("vmsgtu.vx v8, v24, %0\n" :"+r"(scalar64):: "v8" , "v16" , "v24");
	asm volatile("vmsgt.vi v8, v24, %0\n" ::"i"(imm): "v8" , "v16" , "v24");
	asm volatile("vmsgt.vx v8, v24, %0\n" :"+r"(scalar64):: "v8" , "v16" , "v24");
	asm volatile("vmsif.m v8, v24\n" ::: "v8" , "v16" , "v24");
	asm volatile("vmsleu.vi v8, v24, %0\n" ::"i"(imm): "v8" , "v16" , "v24");
	asm volatile("vmsleu.vv v8, v24, v16\n" ::: "v8" , "v16" , "v24");
	asm volatile("vmsleu.vx v8, v24, %0\n" :"+r"(scalar64):: "v8" , "v16" , "v24");
	asm volatile("vmsle.vi v8, v24, %0\n" ::"i"(imm): "v8" , "v16" , "v24");
	asm volatile("vmsle.vv v8, v24, v16\n" ::: "v8" , "v16" , "v24");
	asm volatile("vmsle.vx v8, v24, %0\n" :"+r"(scalar64):: "v8" , "v16" , "v24");
	asm volatile("vmsltu.vv v8, v24, v16\n" ::: "v8" , "v16" , "v24");
	asm volatile("vmsltu.vx v8, v24, %0\n" :"+r"(scalar64):: "v8" , "v16" , "v24");
	asm volatile("vmslt.vi v0, v0, 5\n" ::"i"(imm): "v8" , "v16" , "v24");
	asm volatile("vmslt.vv v8, v24, v16\n" ::: "v8" , "v16" , "v24");
	asm volatile("vmslt.vx v8, v24, %0\n" :"+r"(scalar64):: "v8" , "v16" , "v24");
	asm volatile("vmsne.vi v8, v24, %0\n" ::"i"(imm): "v8" , "v16" , "v24");
	asm volatile("vmsne.vv v8, v24, v16\n" ::: "v8" , "v16" , "v24");
	asm volatile("vmsne.vx v8, v24, %0\n" :"+r"(scalar64):: "v8" , "v16" , "v24");
	asm volatile("vmxnor.mm v8, v24, v16\n" ::: "v8" , "v16" , "v24");
	asm volatile("vmxor.mm v8, v24, v16\n" ::: "v8" , "v16" , "v24");
	rave_end_region("Mask creation");

	rave_begin_region("MEM Unit-strided");
	//Unit strided
	vsetvl64(vlmax);
	asm volatile("vlhff.v v1, (%0)\n" ::"r"(array): "v8" , "v16" , "v24");
	asm volatile("vsw.v v8, (%0)\n" ::"r"(array): "v8" , "v16" , "v24");
	asm volatile("vlbff.v v1, (%0)\n" ::"r"(array): "v8" , "v16" , "v24");
	asm volatile("vlbuff.v v8, (%0)\n" ::"r"(array): "v8" , "v16" , "v24");
	asm volatile("vlbu.v v8, (%0)\n" ::"r"(array): "v8" , "v16" , "v24");
	asm volatile("vlb.v v8, (%0)\n" ::"r"(array): "v8" , "v16" , "v24");
	asm volatile("vleff.v v8, (%0)\n" ::"r"(array): "v8" , "v16" , "v24");
	asm volatile("vle.v v8, (%0)\n" ::"r"(array): "v8" , "v16" , "v24");
	asm volatile("vlhuff.v v8, (%0)\n" ::"r"(array): "v8" , "v16" , "v24");
	asm volatile("vlhu.v v8, (%0)\n" ::"r"(array): "v8" , "v16" , "v24");
	asm volatile("vlh.v v8, (%0)\n" ::"r"(array): "v8" , "v16" , "v24");
	asm volatile("vlwff.v v8, (%0)\n" ::"r"(array): "v8" , "v16" , "v24");
	asm volatile("vlwuff.v v8, (%0)\n" ::"r"(array): "v8" , "v16" , "v24");
	asm volatile("vlwu.v v8, (%0)\n" ::"r"(array): "v8" , "v16" , "v24");
	asm volatile("vlw.v v8, (%0)\n" ::"r"(array): "v8" , "v16" , "v24");
	asm volatile("vsb.v v8, (%0)\n" ::"r"(array): "v8" , "v16" , "v24");
	asm volatile("vse.v v8, (%0)\n" ::"r"(array): "v8" , "v16" , "v24");
	asm volatile("vsh.v v8, (%0)\n" ::"r"(array): "v8" , "v16" , "v24");
	rave_end_region("MEM Unit-strided");

	rave_begin_region("MEM strided");
	//Strided
	vsetvl64(vlmax);
	asm volatile("vssw.v v8, (%0), %1\n" ::"r"(array), "r"(stride): "v8" , "v16" , "v24");
	asm volatile("vlsbu.v v8, (%0), %1\n" ::"r"(array), "r"(stride): "v8" , "v16" , "v24");
	asm volatile("vlsb.v v8, (%0), %1\n" ::"r"(array), "r"(stride): "v8" , "v16" , "v24");
	asm volatile("vlse.v v8, (%0), %1\n" ::"r"(array), "r"(stride): "v8" , "v16" , "v24");
	asm volatile("vlshu.v v8, (%0), %1\n" ::"r"(array), "r"(stride): "v8" , "v16" , "v24");
	asm volatile("vlsh.v v8, (%0), %1\n" ::"r"(array), "r"(stride): "v8" , "v16" , "v24");
	asm volatile("vlswu.v v8, (%0), %1\n" ::"r"(array), "r"(stride): "v8" , "v16" , "v24");
	asm volatile("vlsw.v v8, (%0), %1\n" ::"r"(array), "r"(stride): "v8" , "v16" , "v24");
	asm volatile("vssb.v v8, (%0), %1\n" ::"r"(array), "r"(stride): "v8" , "v16" , "v24");
	asm volatile("vsse.v v8, (%0), %1\n" ::"r"(array), "r"(stride): "v8" , "v16" , "v24");
	asm volatile("vssh.v v8, (%0), %1\n" ::"r"(array), "r"(stride): "v8" , "v16" , "v24");
	rave_end_region("MEM strided");

	rave_begin_region("MEM indexed");
	//Indexed
	vsetvl64(vlmax);
	asm volatile("vlxe.v v8, (%0), v24\n" ::"r"(array): "v8" , "v16" , "v24");
	asm volatile("vsuxb.v v8, (%0), v24\n" ::"r"(array): "v8" , "v16" , "v24");
	asm volatile("vsuxe.v v8, (%0), v24\n" ::"r"(array): "v8" , "v16" , "v24");
	asm volatile("vsuxh.v v8, (%0), v24\n" ::"r"(array): "v8" , "v16" , "v24");
	asm volatile("vsuxw.v v8, (%0), v24\n" ::"r"(array): "v8" , "v16" , "v24");
	asm volatile("vsxb.v v8, (%0), v24\n" ::"r"(array): "v8" , "v16" , "v24");
	asm volatile("vsxe.v v8, (%0), v24\n" ::"r"(array): "v8" , "v16" , "v24");
	asm volatile("vsxh.v v8, (%0), v24\n" ::"r"(array): "v8" , "v16" , "v24");
	asm volatile("vsxw.v v8, (%0), v24\n" ::"r"(array): "v8" , "v16" , "v24");
	asm volatile("vlxbu.v v8, (%0), v24\n" ::"r"(array): "v8" , "v16" , "v24");
	asm volatile("vlxb.v v8, (%0), v24\n" ::"r"(array): "v8" , "v16" , "v24");
	asm volatile("vlxhu.v v8, (%0), v24\n" ::"r"(array): "v8" , "v16" , "v24");
	asm volatile("vlxh.v v8, (%0), v24\n" ::"r"(array): "v8" , "v16" , "v24");
	asm volatile("vlxwu.v v8, (%0), v24\n" ::"r"(array): "v8" , "v16" , "v24");
	asm volatile("vlxw.v v8, (%0), v24\n" ::"r"(array): "v8" , "v16" , "v24");
	rave_end_region("MEM indexed");

}
