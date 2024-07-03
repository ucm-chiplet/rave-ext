char startswith(const char * str, const char * find){
	for(int i=0; find[i]!='\0'; ++i){
					if (str[i]=='\0' || find[i]!=str[i]) return 0;
	}
	return 1;
}
int instr2prv(char * i){
	//SetVL
	if (startswith(i, "vsetvli"))			return 1002; 
	if (startswith(i, "vsetvl"))				return 1001; 
	if (startswith(i, "vsetivli"))			return 1003; //1.0
	//I Arithmetic instructions
	if (startswith(i, "vadd."))          return 1; 
	if (startswith(i, "vsub."))          return 2;
	if (startswith(i, "vrsub."))         return 3; 
	if (startswith(i, "vmul."))          return 4; 
	if (startswith(i, "vmulh."))         return 5; 
	if (startswith(i, "vmulhu."))        return 6; 
	if (startswith(i, "vmulhsu."))       return 7; 
	if (startswith(i, "vmadd."))         return 8; 
	if (startswith(i, "vmacc."))         return 9; 
	if (startswith(i, "vdiv."))          return 10; 
	if (startswith(i, "vdivu."))         return 11; 
	if (startswith(i, "vrem."))          return 12; 
	if (startswith(i, "vremu."))         return 13; 

	if (startswith(i, "vsll."))          return 14; 
	if (startswith(i, "vsra."))          return 15; 
	if (startswith(i, "vsrl."))          return 16;

	if (startswith(i, "vand."))          return 17;
	if (startswith(i, "vor."))           return 18;
	if (startswith(i, "vxor."))          return 19;
	if (startswith(i, "vnot."))          return 20;
	if (startswith(i, "vneg"))					return 21; //Only in 1.0

	if (startswith(i, "vredsum."))       return 25; 
	if (startswith(i, "vredand."))       return 26; 
	if (startswith(i, "vredor."))        return 27; 
	if (startswith(i, "vredxor."))       return 28; 
	if (startswith(i, "vredmin."))       return 29; 
	if (startswith(i, "vredminu."))      return 30; 
	if (startswith(i, "vredmax."))       return 31; 
	if (startswith(i, "vredmaxu."))      return 32; 

	if (startswith(i, "vmin."))          return 35; 
	if (startswith(i, "vminu."))         return 36; 
	if (startswith(i, "vmax."))          return 37; 
	if (startswith(i, "vmaxu."))         return 38; 

	if (startswith(i, "vadc."))          return 40; 
	if (startswith(i, "vmadc."))         return 41; 
	if (startswith(i, "vsbc."))          return 42; 
	if (startswith(i, "vmsbc."))         return 43; 

	if (startswith(i, "vsadd."))         return 44; 
	if (startswith(i, "vsaddu."))        return 45; 
	if (startswith(i, "vssub."))         return 46; 
	if (startswith(i, "vssubu."))        return 47; 
	if (startswith(i, "vssrl."))         return 48; 
	if (startswith(i, "vssra."))         return 49; 
	if (startswith(i, "vsmul."))         return 50; 
	if (startswith(i, "vaadd."))         return 51; 
	if (startswith(i, "vasub."))         return 52; 
	if (startswith(i, "vaaddu."))				return 53; //Only in 1.0 
	if (startswith(i, "vasubu."))				return 54; //Only in 1.0 

	if (startswith(i, "vnsrl."))         return 55; 
	if (startswith(i, "vnsra."))         return 56; 
	if (startswith(i, "vnmsac."))        return 57; 
	if (startswith(i, "vnmsub."))        return 58; 

	if (startswith(i, "vwadd."))         return 59; 
	if (startswith(i, "vwaddu."))        return 60; 
	if (startswith(i, "vwsub."))         return 61; 
	if (startswith(i, "vwsubu."))        return 62;
	if (startswith(i, "vwmulu."))        return 63; 
	if (startswith(i, "vwmulsu."))       return 64; 
	if (startswith(i, "vwmul."))         return 65; 
	if (startswith(i, "vwmacc."))        return 66; 
	if (startswith(i, "vwmaccu."))       return 67; 
	if (startswith(i, "vwmaccsu."))      return 68; 
	if (startswith(i, "vwmaccus."))      return 69; 
	if (startswith(i, "vwredsumu."))     return 74; 
	if (startswith(i, "vwredsum."))      return 75; 
	
	
	//F Arithmetic
	if (startswith(i, "vfadd."))         return 100; 
	if (startswith(i, "vfmadd."))        return 101; 
	if (startswith(i, "vfmsub."))        return 102; 
	if (startswith(i, "vfmul."))         return 105; 
		if (startswith(i, "vfredusum."))      return 106;


	if (startswith(i, "vfsub."))         return 107; 
	if (startswith(i, "vfmacc."))        return 108; 
	if (startswith(i, "vfsqrt."))        return 109; 
	if (startswith(i, "vfsgnjn."))       return 110; 
	if (startswith(i, "vfmsac."))        return 111; 
	if (startswith(i, "vfdiv."))         return 112; 
	if (startswith(i, "vfredosum."))     return 113; 
	if (startswith(i, "vfmin."))         return 114; 
	if (startswith(i, "vfredmin."))      return 115; 
	if (startswith(i, "vfmax."))         return 116; 
	if (startswith(i, "vfredmax."))      return 117; 
	if (startswith(i, "vfsgnj."))        return 118; 
	if (startswith(i, "vfsgnjx."))       return 119; 
	if (startswith(i, "vfrdiv."))        return 120; 
	if (startswith(i, "vfnmadd."))       return 121; 
	if (startswith(i, "vfnmsub."))       return 122; 
	if (startswith(i, "vfnmacc."))       return 123; 
	if (startswith(i, "vfnmsac."))       return 124; 
	if (startswith(i, "vfwadd."))        return 125; 
		if (startswith(i, "vfwredusum."))     return 126; //Only in 1.0 
	if (startswith(i, "vfwsub."))        return 127; 
	if (startswith(i, "vfwmul."))        return 129; 
	if (startswith(i, "vfwmacc."))       return 131; 
	if (startswith(i, "vfwnmacc."))      return 132; 
	if (startswith(i, "vfwmsac."))       return 133; 
	if (startswith(i, "vfwnmsac."))      return 134; 
	if (startswith(i, "vfrsub."))        return 135; 
	if (startswith(i, "vfrec"))					return 136; //Only in 1.0 
	if (startswith(i, "vfrsqrt"))				return 137; //Only in 1.0
	if (startswith(i, "vfneg"))					return 138; //Only in 1.0
	if (startswith(i, "vfabs"))					return 139; //Only in 1.0

	//Other
	if (startswith(i, "vid."))           return 200;
	if (startswith(i, "vmv."))           return 201; 
	if (startswith(i, "vfmv."))          return 202; 
	if (startswith(i, "vrgather."))      return 203; 
	if (startswith(i, "vrgatherei16."))	 return 204;
	if (startswith(i, "vmv1r."))         return 205; 
	if (startswith(i, "vmv2r."))					return 206;
	if (startswith(i, "vmv4r."))					return 207;
	if (startswith(i, "vmv8r."))					return 208;

	if (startswith(i, "vmerge."))        return 210; 
	if (startswith(i, "vfmerge."))       return 211; 

	if (startswith(i, "vslidedown."))    return 220; 
	if (startswith(i, "vslide1down."))   return 221; 
	if (startswith(i, "vslideup."))      return 222; 
	if (startswith(i, "vslide1up."))     return 223; 
	if (startswith(i, "vfslide1up."))     return 224; //Only in 1.0 
	if (startswith(i, "vfslide1down."))     return 225; //Only in 1.0 


		if (startswith(i, "vcpop."))        return 231; //Only in 1.0 
		if (startswith(i, "vfirst."))       return 232; //Only in 1.0;
	if (startswith(i, "vcompress."))     return 233; 
	if (startswith(i, "vzext"))					return 234; //Only in 1.0
	if (startswith(i, "vsext."))          return 235; //Only in 1.0 
	if (startswith(i, "vzip2"))					return 236; //Only in 1.0
	if (startswith(i, "vunzip2"))				return 237; //Only in 1.0	
	if (startswith(i, "vtrn"))					return 238; //Only in 1.0

	if (startswith(i, "vwcvt."))         return 240; 
	if (startswith(i, "vwcvtu."))        return 241; 
	if (startswith(i, "vfcvt."))         return 242; 
	if (startswith(i, "vfwcvt."))        return 243; 
	if (startswith(i, "vfncvt."))        return 244; 
	if (startswith(i, "vnclip."))        return 245; 
	if (startswith(i, "vnclipu."))       return 246; 
	if (startswith(i, "vfclass."))       return 247; 
	if (startswith(i, "vncvt."))					return 248; //Only in 1.0

	if (startswith(i, "vmsbf."))         return 250; 
	if (startswith(i, "vmsof."))         return 251; 
	if (startswith(i, "vmsif."))         return 252; 
	if (startswith(i, "viota."))         return 253; 

	

	//Mask
	if (startswith(i, "vmseq."))         return 300;
	if (startswith(i, "vmsne."))         return 301; 
	if (startswith(i, "vmsle."))         return 302; 
	if (startswith(i, "vmslt."))         return 303; 
	if (startswith(i, "vmsltu."))        return 304; 
	if (startswith(i, "vmsleu."))        return 305; 
	if (startswith(i, "vmsgt."))         return 306; 
	if (startswith(i, "vmsgtu."))        return 307; 
	if (startswith(i, "vmsge."))         return 308; 
	if (startswith(i, "vmsgeu."))        return 309; 
	if (startswith(i, "vmfeq."))         return 310; 
	if (startswith(i, "vmfne."))         return 311; 
	if (startswith(i, "vmfle."))         return 312; 
	if (startswith(i, "vmflt."))         return 313; 
	if (startswith(i, "vmfgt."))         return 314; 
	if (startswith(i, "vmfge."))         return 315; 
	if (startswith(i, "vmnot."))         return 316; 
	if (startswith(i, "vmand."))         return 317; 
		if (startswith(i, "vmandn."))      return 318; //Only in 1.0 
	if (startswith(i, "vmnand."))        return 319; 
	if (startswith(i, "vmor."))          return 320; 
		if (startswith(i, "vmorn."))       return 321; //Only in 1.0 
	if (startswith(i, "vmnor."))         return 322; 
	if (startswith(i, "vmxor."))         return 323; 
	if (startswith(i, "vmxnor."))        return 324; 
	if (startswith(i, "vmclr."))         return 325; 
		if (startswith(i, "vmmv."))         return 326; //Only in 1.0 
	if (startswith(i, "vmset."))         return 327; 


/*************
*************/

	//Memory
	if (startswith(i, "vle8."))					return 418;
	if (startswith(i, "vle16."))				return 407;
	if (startswith(i, "vle32."))				return 410;
	if (startswith(i, "vle64."))				return 409;

	if (startswith(i, "vlse8."))					return 403;
	if (startswith(i, "vlse16."))					return 414;
	if (startswith(i, "vlse32."))					return 417;
	if (startswith(i, "vlse64."))					return 406;

	if (startswith(i, "vluxei8."))					return 401;
	if (startswith(i, "vluxei16."))					return 405;
	if (startswith(i, "vluxei32."))					return 402;
	if (startswith(i, "vluxei64."))					return 400;
	if (startswith(i, "vloxei8."))					return 419;
	if (startswith(i, "vloxei16."))					return 416;
	if (startswith(i, "vloxei32."))					return 404;
	if (startswith(i, "vloxei64."))					return 408;

	if (startswith(i, "vlm."))					return 420;
	if (startswith(i, "vl1r."))         return 421; //1.0
	if (startswith(i, "vl2r"))					return 422; //Only in 1.0
	if (startswith(i, "vl4r"))					return 423; //Only in 1.0
	if (startswith(i, "vl8r"))					return 424; //Only in 1.0

	if (startswith(i, "vle8ff."))				return 425;
	if (startswith(i, "vle16ff."))			return 426;
	if (startswith(i, "vle32ff."))			return 427;
	if (startswith(i, "vle64ff."))			return 428;


	if (startswith(i, "vl1re16."))			return 429;
	if (startswith(i, "vl1re32."))			return 430;
	if (startswith(i, "vl1re64."))			return 431;
	if (startswith(i, "vl1re8."))				return 432;
	if (startswith(i, "vl2re16."))			return 433;
	if (startswith(i, "vl2re32."))			return 434;
	if (startswith(i, "vl2re64."))			return 435;
	if (startswith(i, "vl2re8."))				return 436;
	if (startswith(i, "vl4re16."))			return 437;
	if (startswith(i, "vl4re32."))			return 438;
	if (startswith(i, "vl4re64."))			return 439;
	if (startswith(i, "vl4re8."))				return 440;
	if (startswith(i, "vl8re16."))			return 441;
	if (startswith(i, "vl8re32."))			return 442;
	if (startswith(i, "vl8re64."))			return 443;
	if (startswith(i, "vl8re8."))				return 444;


	if (startswith(i, "vlseg2e8ff."))					return 445;
	if (startswith(i, "vlseg2e8."))					return 446;
	if (startswith(i, "vlseg2e16ff."))					return 447;
	if (startswith(i, "vlseg2e16."))					return 448;
	if (startswith(i, "vlseg2e32ff."))					return 449;
	if (startswith(i, "vlseg2e32."))					return 450;
	if (startswith(i, "vlseg2e64ff."))					return 451;
	if (startswith(i, "vlseg2e64."))					return 452;
	if (startswith(i, "vlseg3e8ff."))					return 453;
	if (startswith(i, "vlseg3e8."))					return 454;
	if (startswith(i, "vlseg3e16ff."))					return 455;
	if (startswith(i, "vlseg3e16."))					return 456;
	if (startswith(i, "vlseg3e32ff."))					return 457;
	if (startswith(i, "vlseg3e32."))					return 458;
	if (startswith(i, "vlseg3e64ff."))					return 459;
	if (startswith(i, "vlseg3e64."))					return 460;
	if (startswith(i, "vlseg4e8ff."))					return 461;
	if (startswith(i, "vlseg4e8."))					return 462;
	if (startswith(i, "vlseg4e16ff."))					return 463;
	if (startswith(i, "vlseg4e16."))					return 464;
	if (startswith(i, "vlseg4e32ff."))					return 465;
	if (startswith(i, "vlseg4e32."))					return 466;
	if (startswith(i, "vlseg4e64ff."))					return 467;
	if (startswith(i, "vlseg4e64."))					return 468;
	if (startswith(i, "vlseg5e8ff."))					return 469;
	if (startswith(i, "vlseg5e8."))					return 470;
	if (startswith(i, "vlseg5e16ff."))					return 471;
	if (startswith(i, "vlseg5e16."))					return 472;
	if (startswith(i, "vlseg5e32ff."))					return 473;
	if (startswith(i, "vlseg5e32."))					return 474;
	if (startswith(i, "vlseg5e64ff."))					return 475;
	if (startswith(i, "vlseg5e64."))					return 476;
	if (startswith(i, "vlseg6e8ff."))					return 477;
	if (startswith(i, "vlseg6e8."))					return 478;
	if (startswith(i, "vlseg6e16ff."))					return 479;
	if (startswith(i, "vlseg6e16."))					return 480;
	if (startswith(i, "vlseg6e32ff."))					return 481;
	if (startswith(i, "vlseg6e32."))					return 482;
	if (startswith(i, "vlseg6e64ff."))					return 483;
	if (startswith(i, "vlseg6e64."))					return 484;
	if (startswith(i, "vlseg7e8ff."))					return 485;
	if (startswith(i, "vlseg7e8."))					return 486;
	if (startswith(i, "vlseg7e16ff."))					return 487;
	if (startswith(i, "vlseg7e16."))					return 488;
	if (startswith(i, "vlseg7e32ff."))					return 489;
	if (startswith(i, "vlseg7e32."))					return 490;
	if (startswith(i, "vlseg7e64ff."))					return 491;
	if (startswith(i, "vlseg7e64."))					return 492;
	if (startswith(i, "vlseg8e8ff."))					return 493;
	if (startswith(i, "vlseg8e8."))					return 494;
	if (startswith(i, "vlseg8e16ff."))					return 495;
	if (startswith(i, "vlseg8e16."))					return 496;
	if (startswith(i, "vlseg8e32ff."))					return 497;
	if (startswith(i, "vlseg8e32."))					return 498;
	if (startswith(i, "vlseg8e64ff."))					return 499;
	if (startswith(i, "vlseg8e64."))					return 500;

	if (startswith(i, "vlsseg2e8."))					return 501;
	if (startswith(i, "vlsseg2e16."))					return 502;
	if (startswith(i, "vlsseg2e32."))					return 503;
	if (startswith(i, "vlsseg2e64."))					return 504;
	if (startswith(i, "vlsseg3e8."))					return 505;
	if (startswith(i, "vlsseg3e16."))					return 506;
	if (startswith(i, "vlsseg3e32."))					return 507;
	if (startswith(i, "vlsseg3e64."))					return 508;
	if (startswith(i, "vlsseg4e8."))					return 509;
	if (startswith(i, "vlsseg4e16."))					return 510;
	if (startswith(i, "vlsseg4e32."))					return 511;
	if (startswith(i, "vlsseg4e64."))					return 512;
	if (startswith(i, "vlsseg5e8."))					return 513;
	if (startswith(i, "vlsseg5e16."))					return 514;
	if (startswith(i, "vlsseg5e32."))					return 515;
	if (startswith(i, "vlsseg5e64."))					return 516;
	if (startswith(i, "vlsseg6e8."))					return 517;
	if (startswith(i, "vlsseg6e16."))					return 518;
	if (startswith(i, "vlsseg6e32."))					return 519;
	if (startswith(i, "vlsseg6e64."))					return 520;
	if (startswith(i, "vlsseg7e8."))					return 521;
	if (startswith(i, "vlsseg7e16."))					return 522;
	if (startswith(i, "vlsseg7e32."))					return 523;
	if (startswith(i, "vlsseg7e64."))					return 524;
	if (startswith(i, "vlsseg8e8."))					return 525;
	if (startswith(i, "vlsseg8e16."))					return 526;
	if (startswith(i, "vlsseg8e32."))					return 527;
	if (startswith(i, "vlsseg8e64."))					return 528;

	if (startswith(i, "vluxseg2ei8."))					return 529;
	if (startswith(i, "vluxseg2ei16."))					return 530;
	if (startswith(i, "vluxseg2ei32."))					return 531;
	if (startswith(i, "vluxseg2ei64."))					return 532;
	if (startswith(i, "vluxseg3ei8."))					return 533;
	if (startswith(i, "vluxseg3ei16."))					return 534;
	if (startswith(i, "vluxseg3ei32."))					return 535;
	if (startswith(i, "vluxseg3ei64."))					return 536;
	if (startswith(i, "vluxseg4ei8."))					return 537;
	if (startswith(i, "vluxseg4ei16."))					return 538;
	if (startswith(i, "vluxseg4ei32."))					return 539;
	if (startswith(i, "vluxseg4ei64."))					return 540;
	if (startswith(i, "vluxseg5ei8."))					return 541;
	if (startswith(i, "vluxseg5ei16."))					return 538;
	if (startswith(i, "vluxseg5ei32."))					return 543;
	if (startswith(i, "vluxseg5ei64."))					return 544;
	if (startswith(i, "vluxseg6ei8."))					return 545;
	if (startswith(i, "vluxseg6ei16."))					return 546;
	if (startswith(i, "vluxseg6ei32."))					return 547;
	if (startswith(i, "vluxseg6ei64."))					return 548;
	if (startswith(i, "vluxseg7ei8."))					return 549;
	if (startswith(i, "vluxseg7ei16."))					return 550;
	if (startswith(i, "vluxseg7ei32."))					return 551;
	if (startswith(i, "vluxseg7ei64."))					return 552;
	if (startswith(i, "vluxseg8ei8."))					return 553;
	if (startswith(i, "vluxseg8ei16."))					return 554;
	if (startswith(i, "vluxseg8ei32."))					return 555;
	if (startswith(i, "vluxseg8ei64."))					return 556;

	if (startswith(i, "vloxseg2ei8."))					return 557;
	if (startswith(i, "vloxseg2ei16."))					return 558;
	if (startswith(i, "vloxseg2ei32."))					return 559;
	if (startswith(i, "vloxseg2ei64."))					return 560;
	if (startswith(i, "vloxseg3ei8."))					return 561;
	if (startswith(i, "vloxseg3ei16."))					return 562;
	if (startswith(i, "vloxseg3ei32."))					return 563;
	if (startswith(i, "vloxseg3ei64."))					return 564;
	if (startswith(i, "vloxseg4ei8."))					return 565;
	if (startswith(i, "vloxseg4ei16."))					return 566;
	if (startswith(i, "vloxseg4ei32."))					return 567;
	if (startswith(i, "vloxseg4ei64."))					return 568;
	if (startswith(i, "vloxseg5ei8."))					return 569;
	if (startswith(i, "vloxseg5ei16."))					return 570;
	if (startswith(i, "vloxseg5ei32."))					return 571;
	if (startswith(i, "vloxseg5ei64."))					return 572;
	if (startswith(i, "vloxseg6ei8."))					return 573;
	if (startswith(i, "vloxseg6ei16."))					return 574;
	if (startswith(i, "vloxseg6ei32."))					return 575;
	if (startswith(i, "vloxseg6ei64."))					return 576;
	if (startswith(i, "vloxseg7ei8."))					return 577;
	if (startswith(i, "vloxseg7ei16."))					return 578;
	if (startswith(i, "vloxseg7ei32."))					return 579;
	if (startswith(i, "vloxseg7ei64."))					return 580;
	if (startswith(i, "vloxseg8ei8."))					return 581;
	if (startswith(i, "vloxseg8ei16."))					return 582;
	if (startswith(i, "vloxseg8ei32."))					return 583;
	if (startswith(i, "vloxseg8ei64."))					return 584;



	
	if (startswith(i, "vse8."))					return 809;
	if (startswith(i, "vse16."))					return 806;
	if (startswith(i, "vse32."))					return 817;
	if (startswith(i, "vse64."))					return 813;
	if (startswith(i, "vsse8."))					return 801;
	if (startswith(i, "vsse16."))					return 812;
	if (startswith(i, "vsse32."))					return 814;
	if (startswith(i, "vsse64."))					return 804;
	if (startswith(i, "vsuxei8."))					return 800;
	if (startswith(i, "vsuxei16."))					return 807;
	if (startswith(i, "vsuxei32."))					return 821;
	if (startswith(i, "vsuxei64."))					return 808;

	if (startswith(i, "vsoxei8."))					return 802;
	if (startswith(i, "vsoxei16."))					return 803;
	if (startswith(i, "vsoxei32."))					return 811;
	if (startswith(i, "vsoxei64."))					return 815;



	if (startswith(i, "vsm."))					return 820;
//	if (startswith(i, "vse1."))					return 14;
	if (startswith(i, "vs1r."))          return 821; //1.0
	if (startswith(i, "vs2r."))					return 822;
	if (startswith(i, "vs4r."))					return 823;
	if (startswith(i, "vs8r."))					return 824;

	if (startswith(i, "vsseg2e8."))					return 825;
	if (startswith(i, "vsseg2e16."))					return 826;
	if (startswith(i, "vsseg2e32."))					return 827;
	if (startswith(i, "vsseg2e64."))					return 828;
	if (startswith(i, "vsseg3e8."))					return 829;
	if (startswith(i, "vsseg3e16."))					return 830;
	if (startswith(i, "vsseg3e32."))					return 831;
	if (startswith(i, "vsseg3e64."))					return 832;
	if (startswith(i, "vsseg4e8."))					return 833;
	if (startswith(i, "vsseg4e16."))					return 834;
	if (startswith(i, "vsseg4e32."))					return 835;
	if (startswith(i, "vsseg4e64."))					return 836;
	if (startswith(i, "vsseg5e8."))					return 837;
	if (startswith(i, "vsseg5e16."))					return 838;
	if (startswith(i, "vsseg5e32."))					return 839;
	if (startswith(i, "vsseg5e64."))					return 840;
	if (startswith(i, "vsseg6e8."))					return 841;
	if (startswith(i, "vsseg6e16."))					return 842;
	if (startswith(i, "vsseg6e32."))					return 843;
	if (startswith(i, "vsseg6e64."))					return 844;
	if (startswith(i, "vsseg7e8."))					return 845;
	if (startswith(i, "vsseg7e16."))					return 846;
	if (startswith(i, "vsseg7e32."))					return 847;
	if (startswith(i, "vsseg7e64."))					return 848;
	if (startswith(i, "vsseg8e8."))					return 849;
	if (startswith(i, "vsseg8e16."))					return 850;
	if (startswith(i, "vsseg8e32."))					return 851;
	if (startswith(i, "vsseg8e64."))					return 852;
	if (startswith(i, "vssseg2e8."))					return 853;
	if (startswith(i, "vssseg2e16."))					return 854;
	if (startswith(i, "vssseg2e32."))					return 855;
	if (startswith(i, "vssseg2e64."))					return 856;
	if (startswith(i, "vssseg3e8."))					return 857;
	if (startswith(i, "vssseg3e16."))					return 858;
	if (startswith(i, "vssseg3e32."))					return 859;
	if (startswith(i, "vssseg3e64."))					return 860;
	if (startswith(i, "vssseg4e8."))					return 861;
	if (startswith(i, "vssseg4e16."))					return 862;
	if (startswith(i, "vssseg4e32."))					return 863;
	if (startswith(i, "vssseg4e64."))					return 864;
	if (startswith(i, "vssseg5e8."))					return 865;
	if (startswith(i, "vssseg5e16."))					return 866;
	if (startswith(i, "vssseg5e32."))					return 867;
	if (startswith(i, "vssseg5e64."))					return 868;
	if (startswith(i, "vssseg6e8."))					return 869;
	if (startswith(i, "vssseg6e16."))					return 870;
	if (startswith(i, "vssseg6e32."))					return 871;
	if (startswith(i, "vssseg6e64."))					return 872;
	if (startswith(i, "vssseg7e8."))					return 869;
	if (startswith(i, "vssseg7e16."))					return 874;
	if (startswith(i, "vssseg7e32."))					return 875;
	if (startswith(i, "vssseg7e64."))					return 876;
	if (startswith(i, "vssseg8e8."))					return 877;
	if (startswith(i, "vssseg8e16."))					return 878;
	if (startswith(i, "vssseg8e32."))					return 879;
	if (startswith(i, "vssseg8e64."))					return 880;
	if (startswith(i, "vsuxseg2ei8."))					return 881;
	if (startswith(i, "vsuxseg2ei16."))					return 882;
	if (startswith(i, "vsuxseg2ei32."))					return 883;
	if (startswith(i, "vsuxseg2ei64."))					return 884;
	if (startswith(i, "vsuxseg3ei8."))					return 885;
	if (startswith(i, "vsuxseg3ei16."))					return 886;
	if (startswith(i, "vsuxseg3ei32."))					return 887;
	if (startswith(i, "vsuxseg3ei64."))					return 888;
	if (startswith(i, "vsuxseg4ei8."))					return 889;
	if (startswith(i, "vsuxseg4ei16."))					return 890;
	if (startswith(i, "vsuxseg4ei32."))					return 891;
	if (startswith(i, "vsuxseg4ei64."))					return 892;
	if (startswith(i, "vsuxseg5ei8."))					return 893;
	if (startswith(i, "vsuxseg5ei16."))					return 894;
	if (startswith(i, "vsuxseg5ei32."))					return 895;
	if (startswith(i, "vsuxseg5ei64."))					return 896;
	if (startswith(i, "vsuxseg6ei8."))					return 897;
	if (startswith(i, "vsuxseg6ei16."))					return 898;
	if (startswith(i, "vsuxseg6ei32."))					return 899;
	if (startswith(i, "vsuxseg6ei64."))					return 900;
	if (startswith(i, "vsuxseg7ei8."))					return 901;
	if (startswith(i, "vsuxseg7ei16."))					return 902;
	if (startswith(i, "vsuxseg7ei32."))					return 903;
	if (startswith(i, "vsuxseg7ei64."))					return 904;
	if (startswith(i, "vsuxseg8ei8."))					return 905;
	if (startswith(i, "vsuxseg8ei16."))					return 906;
	if (startswith(i, "vsuxseg8ei32."))					return 907;
	if (startswith(i, "vsuxseg8ei64."))					return 908;

	if (startswith(i, "vsoxseg2ei8."))					return 909;
	if (startswith(i, "vsoxseg2ei16."))					return 910;
	if (startswith(i, "vsoxseg2ei32."))					return 911;
	if (startswith(i, "vsoxseg2ei64."))					return 912;
	if (startswith(i, "vsoxseg3ei8."))					return 913;
	if (startswith(i, "vsoxseg3ei16."))					return 914;
	if (startswith(i, "vsoxseg3ei32."))					return 915;
	if (startswith(i, "vsoxseg3ei64."))					return 916;
	if (startswith(i, "vsoxseg4ei8."))					return 917;
	if (startswith(i, "vsoxseg4ei16."))					return 918;
	if (startswith(i, "vsoxseg4ei32."))					return 919;
	if (startswith(i, "vsoxseg4ei64."))					return 920;
	if (startswith(i, "vsoxseg5ei8."))					return 921;
	if (startswith(i, "vsoxseg5ei16."))					return 922;
	if (startswith(i, "vsoxseg5ei32."))					return 923;
	if (startswith(i, "vsoxseg5ei64."))					return 924;
	if (startswith(i, "vsoxseg6ei8."))					return 925;
	if (startswith(i, "vsoxseg6ei16."))					return 926;
	if (startswith(i, "vsoxseg6ei32."))					return 927;
	if (startswith(i, "vsoxseg6ei64."))					return 928;
	if (startswith(i, "vsoxseg7ei8."))					return 929;
	if (startswith(i, "vsoxseg7ei16."))					return 930;
	if (startswith(i, "vsoxseg7ei32."))					return 931;
	if (startswith(i, "vsoxseg7ei64."))					return 932;
	if (startswith(i, "vsoxseg8ei8."))					return 933;
	if (startswith(i, "vsoxseg8ei16."))					return 934;
	if (startswith(i, "vsoxseg8ei32."))					return 935;
	if (startswith(i, "vsoxseg8ei64."))					return 936;

	return 999;
}
