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
	if (startswith(i, "vwsmaccsu."))     return 70; //Only 0.7 
	if (startswith(i, "vwsmaccus."))     return 71; //Only 0.7
	if (startswith(i, "vwsmaccu."))      return 72; //Only 0.7
	if (startswith(i, "vwsmacc."))       return 73; //Only 0.7
	if (startswith(i, "vwredsumu."))     return 74; 
	if (startswith(i, "vwredsum."))      return 75; 
	
	//Atomic
	if (startswith(i, "vamoadd"))       return 80; //Only 0.7
	if (startswith(i, "vamoand"))       return 81; //Only 0.7 
	if (startswith(i, "vamoor"))        return 82; //Only 0.7 
	if (startswith(i, "vamoswap"))      return 83; //Only 0.7 
	if (startswith(i, "vamoxor"))       return 84; //Only 0.7 
	if (startswith(i, "vamomin"))       return 85; //Only 0.7 
	if (startswith(i, "vamomax"))       return 86; //Only 0.7 
	
	//F Arithmetic
	if (startswith(i, "vfadd."))         return 100; 
	if (startswith(i, "vfmadd."))        return 101; 
	if (startswith(i, "vfmsub."))        return 102; 
	if (startswith(i, "vfmul."))         return 105; 
	if (startswith(i, "vfredsum."))      return 106; //Only 0.7 
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
	if (startswith(i, "vfwredsum."))     return 126; 
	if (startswith(i, "vfwsub."))        return 127; 
	if (startswith(i, "vfwredosum."))    return 128; //Only 0.7
	if (startswith(i, "vfwmul."))        return 129; 
	if (startswith(i, "vfdot."))         return 130; //Only 0.7
	if (startswith(i, "vfwmacc."))       return 131; 
	if (startswith(i, "vfwnmacc."))      return 132; 
	if (startswith(i, "vfwmsac."))       return 133; 
	if (startswith(i, "vfwnmsac."))      return 134; 
	if (startswith(i, "vfrsub."))        return 135; 

	//Other
	if (startswith(i, "vid."))           return 200;
	if (startswith(i, "vmv."))           return 201; 
	if (startswith(i, "vfmv."))          return 202; 
	if (startswith(i, "vrgather."))      return 203; 

	if (startswith(i, "vmerge."))        return 210; 
	if (startswith(i, "vfmerge."))       return 211; 

	if (startswith(i, "vslidedown."))    return 220; 
	if (startswith(i, "vslide1down."))   return 221; 
	if (startswith(i, "vslideup."))      return 222; 
	if (startswith(i, "vslide1up."))     return 223; 


	if (startswith(i, "vext."))          return 230; //Only 0.7 
	if (startswith(i, "vmpopc."))        return 231; //Only 0.7
	if (startswith(i, "vmfirst."))       return 232; 
	if (startswith(i, "vcompress."))     return 233; 

	if (startswith(i, "vwcvt."))         return 240; 
	if (startswith(i, "vwcvtu."))        return 241; 
	if (startswith(i, "vfcvt."))         return 242; 
	if (startswith(i, "vfwcvt."))        return 243; 
	if (startswith(i, "vfncvt."))        return 244; 
	if (startswith(i, "vnclip."))        return 245; 
	if (startswith(i, "vnclipu."))       return 246; 
	if (startswith(i, "vfclass."))       return 247; 

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
	if (startswith(i, "vmandnot."))      return 318; //Only 0.7 
	if (startswith(i, "vmnand."))        return 319; 
	if (startswith(i, "vmor."))          return 320; 
	if (startswith(i, "vmornot."))       return 321; //Only 0.7
	if (startswith(i, "vmnor."))         return 322; 
	if (startswith(i, "vmxor."))         return 323; 
	if (startswith(i, "vmxnor."))        return 324; 
	if (startswith(i, "vmclr."))         return 325; 
	if (startswith(i, "vmcpy."))         return 326; 
	if (startswith(i, "vmset."))         return 327; 
	if (startswith(i, "vmford."))        return 328; //Only 0.7
	
	//Memory
	if (startswith(i, "vle."))           return 400;//0.7 
	if (startswith(i, "vlse."))          return 401;// 0.7
	if (startswith(i, "vlxe."))          return 402;//0.7

	if (startswith(i, "vlb."))           return 403;// 0.7
	if (startswith(i, "vlbu."))          return 404;// 0.7
	if (startswith(i, "vlh."))           return 405;// 0.7
	if (startswith(i, "vlhu."))          return 406;// 0.7
	if (startswith(i, "vlw."))           return 407;// 0.7
	if (startswith(i, "vlwu."))          return 408;// 0.7
	if (startswith(i, "vlsb."))          return 409;// 0.7
	if (startswith(i, "vlsbu."))         return 410;// 0.7
	if (startswith(i, "vlsh."))          return 411;// 0.7
	if (startswith(i, "vlshu."))         return 412;// 0.7
	if (startswith(i, "vlsw."))          return 413;// 0.7
	if (startswith(i, "vlswu."))         return 414;// 0.7	
	if (startswith(i, "vlxb."))          return 415;// 0.7
	if (startswith(i, "vlxbu."))         return 416;// 0.7
	if (startswith(i, "vlxh."))          return 417;// 0.7
	if (startswith(i, "vlxhu."))         return 418;// 0.7
	if (startswith(i, "vlxw."))          return 419;// 0.7
	if (startswith(i, "vlxwu."))         return 420;// 0.7

	if (startswith(i, "vleff."))         return 421; //0.7 
	if (startswith(i, "vlbff."))         return 422; //0.7 
	if (startswith(i, "vlbuff."))        return 423; //0.7 
	if (startswith(i, "vlhff."))         return 424; //0.7 
	if (startswith(i, "vlhuff."))        return 425; //0.7 
	if (startswith(i, "vlwff."))         return 426; //0.7
	if (startswith(i, "vlwuff."))        return 427; //0.7 

	if (startswith(i, "vlseg1b."))       return 428; 
	if (startswith(i, "vlseg1bu."))      return 429; 
	if (startswith(i, "vlseg1e."))       return 430; 
	if (startswith(i, "vlseg1h."))       return 431; 
	if (startswith(i, "vlseg1hu."))      return 432; 
	if (startswith(i, "vlseg1w."))       return 433; 
	if (startswith(i, "vlseg1wu."))      return 434; 
	if (startswith(i, "vlseg2b."))       return 435; 
	if (startswith(i, "vlseg2bu."))      return 436; 
	if (startswith(i, "vlseg2e."))       return 437; 
	if (startswith(i, "vlseg2h."))       return 438; 
	if (startswith(i, "vlseg2hu."))      return 439; 
	if (startswith(i, "vlseg2w."))       return 440; 
	if (startswith(i, "vlseg2wu."))      return 441; 
	if (startswith(i, "vlseg3b."))       return 442; 
	if (startswith(i, "vlseg3bu."))      return 443; 
	if (startswith(i, "vlseg3e."))       return 444; 
	if (startswith(i, "vlseg3h."))       return 445; 
	if (startswith(i, "vlseg3hu."))      return 446; 
	if (startswith(i, "vlseg3w."))       return 447; 
	if (startswith(i, "vlseg3wu."))      return 448; 
	if (startswith(i, "vlseg4b."))       return 449; 
	if (startswith(i, "vlseg4bu."))      return 450; 
	if (startswith(i, "vlseg4e."))       return 451; 
	if (startswith(i, "vlseg4h."))       return 452; 
	if (startswith(i, "vlseg4hu."))      return 453; 
	if (startswith(i, "vlseg4w."))       return 454; 
	if (startswith(i, "vlseg4wu."))      return 455; 
	if (startswith(i, "vlseg5b."))       return 456; 
	if (startswith(i, "vlseg5bu."))      return 457; 
	if (startswith(i, "vlseg5e."))       return 458; 
	if (startswith(i, "vlseg5h."))       return 459; 
	if (startswith(i, "vlseg5hu."))      return 460; 
	if (startswith(i, "vlseg5w."))       return 461; 
	if (startswith(i, "vlseg5wu."))      return 462; 
	if (startswith(i, "vlseg6b."))       return 463; 
	if (startswith(i, "vlseg6bu."))      return 464; 
	if (startswith(i, "vlseg6e."))       return 465; 
	if (startswith(i, "vlseg6h."))       return 466; 
	if (startswith(i, "vlseg6hu."))      return 467; 
	if (startswith(i, "vlseg6w."))       return 468; 
	if (startswith(i, "vlseg6wu."))      return 469; 
	if (startswith(i, "vlseg7b."))       return 470; 
	if (startswith(i, "vlseg7bu."))      return 471; 
	if (startswith(i, "vlseg7e."))       return 472; 
	if (startswith(i, "vlseg7h."))       return 473; 
	if (startswith(i, "vlseg7hu."))      return 474; 
	if (startswith(i, "vlseg7w."))       return 475; 
	if (startswith(i, "vlseg7wu."))      return 476; 
	if (startswith(i, "vlseg8b."))       return 477; 
	if (startswith(i, "vlseg8bu."))      return 478; 
	if (startswith(i, "vlseg8e."))       return 479; 
	if (startswith(i, "vlseg8h."))       return 480; 
	if (startswith(i, "vlseg8hu."))      return 481; 
	if (startswith(i, "vlseg8w."))       return 482; 
	if (startswith(i, "vlseg8wu."))      return 483; 
	if (startswith(i, "vlsseg1b."))      return 484; 
	if (startswith(i, "vlsseg1bu."))     return 485; 
	if (startswith(i, "vlsseg1e."))      return 486; 
	if (startswith(i, "vlsseg1h."))      return 487; 
	if (startswith(i, "vlsseg1hu."))     return 488; 
	if (startswith(i, "vlsseg1w."))      return 489; 
	if (startswith(i, "vlsseg1wu."))     return 490; 
	if (startswith(i, "vlsseg2b."))      return 491; 
	if (startswith(i, "vlsseg2bu."))     return 492; 
	if (startswith(i, "vlsseg2e."))      return 493; 
	if (startswith(i, "vlsseg2h."))      return 494; 
	if (startswith(i, "vlsseg2hu."))     return 495; 
	if (startswith(i, "vlsseg2w."))      return 496; 
	if (startswith(i, "vlsseg2wu."))     return 497; 
	if (startswith(i, "vlsseg3b."))      return 498; 
	if (startswith(i, "vlsseg3bu."))     return 499; 
	if (startswith(i, "vlsseg3e."))      return 500; 
	if (startswith(i, "vlsseg3h."))      return 501; 
	if (startswith(i, "vlsseg3hu."))     return 502; 
	if (startswith(i, "vlsseg3w."))      return 503; 
	if (startswith(i, "vlsseg3wu."))     return 504; 
	if (startswith(i, "vlsseg4b."))      return 505; 
	if (startswith(i, "vlsseg4bu."))     return 506; 
	if (startswith(i, "vlsseg4e."))      return 507; 
	if (startswith(i, "vlsseg4h."))      return 508; 
	if (startswith(i, "vlsseg4hu."))     return 509; 
	if (startswith(i, "vlsseg4w."))      return 510; 
	if (startswith(i, "vlsseg4wu."))     return 511; 
	if (startswith(i, "vlsseg5b."))      return 512; 
	if (startswith(i, "vlsseg5bu."))     return 513; 
	if (startswith(i, "vlsseg5e."))      return 514; 
	if (startswith(i, "vlsseg5h."))      return 515; 
	if (startswith(i, "vlsseg5hu."))     return 516; 
	if (startswith(i, "vlsseg5w."))      return 517; 
	if (startswith(i, "vlsseg5wu."))     return 518; 
	if (startswith(i, "vlsseg6b."))      return 519; 
	if (startswith(i, "vlsseg6bu."))     return 520; 
	if (startswith(i, "vlsseg6e."))      return 521; 
	if (startswith(i, "vlsseg6h."))      return 522; 
	if (startswith(i, "vlsseg6hu."))     return 523; 
	if (startswith(i, "vlsseg6w."))      return 524; 
	if (startswith(i, "vlsseg6wu."))     return 525; 
	if (startswith(i, "vlsseg7b."))      return 526; 
	if (startswith(i, "vlsseg7bu."))     return 527; 
	if (startswith(i, "vlsseg7e."))      return 528; 
	if (startswith(i, "vlsseg7h."))      return 529; 
	if (startswith(i, "vlsseg7hu."))     return 530; 
	if (startswith(i, "vlsseg7w."))      return 531; 
	if (startswith(i, "vlsseg7wu."))     return 532; 
	if (startswith(i, "vlsseg8b."))      return 533; 
	if (startswith(i, "vlsseg8bu."))     return 534; 
	if (startswith(i, "vlsseg8e."))      return 535; 
	if (startswith(i, "vlsseg8h."))      return 536; 
	if (startswith(i, "vlsseg8hu."))     return 537; 
	if (startswith(i, "vlsseg8w."))      return 538; 
	if (startswith(i, "vlsseg8wu."))     return 539; 
	if (startswith(i, "vlxseg1b."))      return 540; 
	if (startswith(i, "vlxseg1bu."))     return 541; 
	if (startswith(i, "vlxseg1e."))      return 542; 
	if (startswith(i, "vlxseg1h."))      return 543; 
	if (startswith(i, "vlxseg1hu."))     return 544; 
	if (startswith(i, "vlxseg1w."))      return 545; 
	if (startswith(i, "vlxseg1wu."))     return 546; 
	if (startswith(i, "vlxseg2b."))      return 547; 
	if (startswith(i, "vlxseg2bu."))     return 548; 
	if (startswith(i, "vlxseg2e."))      return 549; 
	if (startswith(i, "vlxseg2h."))      return 550; 
	if (startswith(i, "vlxseg2hu."))     return 551; 
	if (startswith(i, "vlxseg2w."))      return 552; 
	if (startswith(i, "vlxseg2wu."))     return 553; 
	if (startswith(i, "vlxseg3b."))      return 554; 
	if (startswith(i, "vlxseg3bu."))     return 555; 
	if (startswith(i, "vlxseg3e."))      return 556; 
	if (startswith(i, "vlxseg3h."))      return 557; 
	if (startswith(i, "vlxseg3hu."))     return 558; 
	if (startswith(i, "vlxseg3w."))      return 559; 
	if (startswith(i, "vlxseg3wu."))     return 560; 
	if (startswith(i, "vlxseg4b."))      return 561; 
	if (startswith(i, "vlxseg4bu."))     return 562; 
	if (startswith(i, "vlxseg4e."))      return 563; 
	if (startswith(i, "vlxseg4h."))      return 564; 
	if (startswith(i, "vlxseg4hu."))     return 565; 
	if (startswith(i, "vlxseg4w."))      return 566; 
	if (startswith(i, "vlxseg4wu."))     return 567; 
	if (startswith(i, "vlxseg5b."))      return 568; 
	if (startswith(i, "vlxseg5bu."))     return 569; 
	if (startswith(i, "vlxseg5e."))      return 570; 
	if (startswith(i, "vlxseg5h."))      return 571; 
	if (startswith(i, "vlxseg5hu."))     return 572; 
	if (startswith(i, "vlxseg5w."))      return 573; 
	if (startswith(i, "vlxseg5wu."))     return 574; 
	if (startswith(i, "vlxseg6b."))      return 575; 
	if (startswith(i, "vlxseg6bu."))     return 576; 
	if (startswith(i, "vlxseg6e."))      return 577; 
	if (startswith(i, "vlxseg6h."))      return 578; 
	if (startswith(i, "vlxseg6hu."))     return 579; 
	if (startswith(i, "vlxseg6w."))      return 580; 
	if (startswith(i, "vlxseg6wu."))     return 581; 
	if (startswith(i, "vlxseg7b."))      return 582; 
	if (startswith(i, "vlxseg7bu."))     return 583; 
	if (startswith(i, "vlxseg7e."))      return 584; 
	if (startswith(i, "vlxseg7h."))      return 585; 
	if (startswith(i, "vlxseg7hu."))     return 586; 
	if (startswith(i, "vlxseg7w."))      return 587; 
	if (startswith(i, "vlxseg7wu."))     return 588; 
	if (startswith(i, "vlxseg8b."))      return 589; 
	if (startswith(i, "vlxseg8bu."))     return 590; 
	if (startswith(i, "vlxseg8e."))      return 591; 
	if (startswith(i, "vlxseg8h."))      return 592; 
	if (startswith(i, "vlxseg8hu."))     return 593; 
	if (startswith(i, "vlxseg8w."))      return 594; 
	if (startswith(i, "vlxseg8wu."))     return 595; 

	if (startswith(i, "vse."))           return 800;//0.7 
	if (startswith(i, "vsse."))          return 801;// 0.7
	if (startswith(i, "vsxe."))          return 802;//0.7
	if (startswith(i, "vsuxe."))         return 803; //0.7

	if (startswith(i, "vsb."))           return 804;// 0.7
	if (startswith(i, "vsh."))           return 805;// 0.7
	if (startswith(i, "vsw."))           return 806;// 0.7
	if (startswith(i, "vssb."))          return 807;// 0.7
	if (startswith(i, "vssh."))          return 808;// 0.7
	if (startswith(i, "vssw."))          return 809;// 0.7
	if (startswith(i, "vsxb."))          return 810;// 0.7
	if (startswith(i, "vsxh."))          return 811;// 0.7
	if (startswith(i, "vsxw."))          return 812;// 0.7
	if (startswith(i, "vsuxb."))         return 813;// 0.7
	if (startswith(i, "vsuxh."))         return 814;// 0.7
	if (startswith(i, "vsuxw."))         return 815;// 0.7

	if (startswith(i, "vsseg1b."))       return 816; 
	if (startswith(i, "vsseg1bu."))      return 817; 
	if (startswith(i, "vsseg1e."))       return 818; 
	if (startswith(i, "vsseg1h."))       return 819; 
	if (startswith(i, "vsseg1hu."))      return 820; 
	if (startswith(i, "vsseg1w."))       return 821; 
	if (startswith(i, "vsseg1wu."))      return 822; 
	if (startswith(i, "vsseg2b."))       return 823; 
	if (startswith(i, "vsseg2bu."))      return 824; 
	if (startswith(i, "vsseg2e."))       return 825; 
	if (startswith(i, "vsseg2h."))       return 826; 
	if (startswith(i, "vsseg2hu."))      return 827; 
	if (startswith(i, "vsseg2w."))       return 828; 
	if (startswith(i, "vsseg2wu."))      return 829; 
	if (startswith(i, "vsseg3b."))       return 830; 
	if (startswith(i, "vsseg3bu."))      return 831; 
	if (startswith(i, "vsseg3e."))       return 832; 
	if (startswith(i, "vsseg3h."))       return 833; 
	if (startswith(i, "vsseg3hu."))      return 834; 
	if (startswith(i, "vsseg3w."))       return 835; 
	if (startswith(i, "vsseg3wu."))      return 836; 
	if (startswith(i, "vsseg4b."))       return 837; 
	if (startswith(i, "vsseg4bu."))      return 838; 
	if (startswith(i, "vsseg4e."))       return 839; 
	if (startswith(i, "vsseg4h."))       return 840; 
	if (startswith(i, "vsseg4hu."))      return 841; 
	if (startswith(i, "vsseg4w."))       return 842; 
	if (startswith(i, "vsseg4wu."))      return 843; 
	if (startswith(i, "vsseg5b."))       return 844; 
	if (startswith(i, "vsseg5bu."))      return 845; 
	if (startswith(i, "vsseg5e."))       return 846; 
	if (startswith(i, "vsseg5h."))       return 847; 
	if (startswith(i, "vsseg5hu."))      return 848; 
	if (startswith(i, "vsseg5w."))       return 849; 
	if (startswith(i, "vsseg5wu."))      return 850; 
	if (startswith(i, "vsseg6b."))       return 851; 
	if (startswith(i, "vsseg6bu."))      return 852; 
	if (startswith(i, "vsseg6e."))       return 853; 
	if (startswith(i, "vsseg6h."))       return 854; 
	if (startswith(i, "vsseg6hu."))      return 855; 
	if (startswith(i, "vsseg6w."))       return 856; 
	if (startswith(i, "vsseg6wu."))      return 857; 
	if (startswith(i, "vsseg7b."))       return 858; 
	if (startswith(i, "vsseg7bu."))      return 859; 
	if (startswith(i, "vsseg7e."))       return 860; 
	if (startswith(i, "vsseg7h."))       return 861; 
	if (startswith(i, "vsseg7hu."))      return 862; 
	if (startswith(i, "vsseg7w."))       return 863; 
	if (startswith(i, "vsseg7wu."))      return 864; 
	if (startswith(i, "vsseg8b."))       return 865; 
	if (startswith(i, "vsseg8bu."))      return 866; 
	if (startswith(i, "vsseg8e."))       return 867; 
	if (startswith(i, "vsseg8h."))       return 868; 
	if (startswith(i, "vsseg8hu."))      return 869; 
	if (startswith(i, "vsseg8w."))       return 870; 
	if (startswith(i, "vsseg8wu."))      return 871; 
	if (startswith(i, "vssseg1b."))      return 872; 
	if (startswith(i, "vssseg1bu."))     return 873; 
	if (startswith(i, "vssseg1e."))      return 874; 
	if (startswith(i, "vssseg1h."))      return 875; 
	if (startswith(i, "vssseg1hu."))     return 876; 
	if (startswith(i, "vssseg1w."))      return 877; 
	if (startswith(i, "vssseg1wu."))     return 878; 
	if (startswith(i, "vssseg2b."))      return 879; 
	if (startswith(i, "vssseg2bu."))     return 880; 
	if (startswith(i, "vssseg2e."))      return 881; 
	if (startswith(i, "vssseg2h."))      return 882; 
	if (startswith(i, "vssseg2hu."))     return 883; 
	if (startswith(i, "vssseg2w."))      return 884; 
	if (startswith(i, "vssseg2wu."))     return 885; 
	if (startswith(i, "vssseg3b."))      return 886; 
	if (startswith(i, "vssseg3bu."))     return 887; 
	if (startswith(i, "vssseg3e."))      return 888; 
	if (startswith(i, "vssseg3h."))      return 889; 
	if (startswith(i, "vssseg3hu."))     return 890; 
	if (startswith(i, "vssseg3w."))      return 891; 
	if (startswith(i, "vssseg3wu."))     return 892; 
	if (startswith(i, "vssseg4b."))      return 893; 
	if (startswith(i, "vssseg4bu."))     return 894; 
	if (startswith(i, "vssseg4e."))      return 895; 
	if (startswith(i, "vssseg4h."))      return 896; 
	if (startswith(i, "vssseg4hu."))     return 897; 
	if (startswith(i, "vssseg4w."))      return 898; 
	if (startswith(i, "vssseg4wu."))     return 899; 
	if (startswith(i, "vssseg5b."))      return 900; 
	if (startswith(i, "vssseg5bu."))     return 901; 
	if (startswith(i, "vssseg5e."))      return 902; 
	if (startswith(i, "vssseg5h."))      return 903; 
	if (startswith(i, "vssseg5hu."))     return 904; 
	if (startswith(i, "vssseg5w."))      return 905; 
	if (startswith(i, "vssseg5wu."))     return 906; 
	if (startswith(i, "vssseg6b."))      return 907; 
	if (startswith(i, "vssseg6bu."))     return 908; 
	if (startswith(i, "vssseg6e."))      return 909; 
	if (startswith(i, "vssseg6h."))      return 910; 
	if (startswith(i, "vssseg6hu."))     return 911; 
	if (startswith(i, "vssseg6w."))      return 912; 
	if (startswith(i, "vssseg6wu."))     return 913; 
	if (startswith(i, "vssseg7b."))      return 914; 
	if (startswith(i, "vssseg7bu."))     return 915; 
	if (startswith(i, "vssseg7e."))      return 916; 
	if (startswith(i, "vssseg7h."))      return 917; 
	if (startswith(i, "vssseg7hu."))     return 918; 
	if (startswith(i, "vssseg7w."))      return 919; 
	if (startswith(i, "vssseg7wu."))     return 920; 
	if (startswith(i, "vssseg8b."))      return 921; 
	if (startswith(i, "vssseg8bu."))     return 922; 
	if (startswith(i, "vssseg8e."))      return 923; 
	if (startswith(i, "vssseg8h."))      return 924; 
	if (startswith(i, "vssseg8hu."))     return 925; 
	if (startswith(i, "vssseg8w."))      return 926; 
	if (startswith(i, "vssseg8wu."))     return 927; 
	if (startswith(i, "vsxseg1b."))      return 928; 
	if (startswith(i, "vsxseg1bu."))     return 929; 
	if (startswith(i, "vsxseg1e."))      return 930; 
	if (startswith(i, "vsxseg1h."))      return 931; 
	if (startswith(i, "vsxseg1hu."))     return 932; 
	if (startswith(i, "vsxseg1w."))      return 933; 
	if (startswith(i, "vsxseg1wu."))     return 934; 
	if (startswith(i, "vsxseg2b."))      return 935; 
	if (startswith(i, "vsxseg2bu."))     return 936; 
	if (startswith(i, "vsxseg2e."))      return 937; 
	if (startswith(i, "vsxseg2h."))      return 938; 
	if (startswith(i, "vsxseg2hu."))     return 939; 
	if (startswith(i, "vsxseg2w."))      return 940; 
	if (startswith(i, "vsxseg2wu."))     return 941; 
	if (startswith(i, "vsxseg3b."))      return 942; 
	if (startswith(i, "vsxseg3bu."))     return 943; 
	if (startswith(i, "vsxseg3e."))      return 944; 
	if (startswith(i, "vsxseg3h."))      return 945; 
	if (startswith(i, "vsxseg3hu."))     return 946; 
	if (startswith(i, "vsxseg3w."))      return 947; 
	if (startswith(i, "vsxseg3wu."))     return 948; 
	if (startswith(i, "vsxseg4b."))      return 949; 
	if (startswith(i, "vsxseg4bu."))     return 950; 
	if (startswith(i, "vsxseg4e."))      return 951; 
	if (startswith(i, "vsxseg4h."))      return 952; 
	if (startswith(i, "vsxseg4hu."))     return 953; 
	if (startswith(i, "vsxseg4w."))      return 954; 
	if (startswith(i, "vsxseg4wu."))     return 955; 
	if (startswith(i, "vsxseg5b."))      return 956; 
	if (startswith(i, "vsxseg5bu."))     return 957; 
	if (startswith(i, "vsxseg5e."))      return 958; 
	if (startswith(i, "vsxseg5h."))      return 959; 
	if (startswith(i, "vsxseg5hu."))     return 960; 
	if (startswith(i, "vsxseg5w."))      return 961; 
	if (startswith(i, "vsxseg5wu."))     return 962; 
	if (startswith(i, "vsxseg6b."))      return 963; 
	if (startswith(i, "vsxseg6bu."))     return 964; 
	if (startswith(i, "vsxseg6e."))      return 965; 
	if (startswith(i, "vsxseg6h."))      return 966; 
	if (startswith(i, "vsxseg6hu."))     return 967; 
	if (startswith(i, "vsxseg6w."))      return 968; 
	if (startswith(i, "vsxseg6wu."))     return 969; 
	if (startswith(i, "vsxseg7b."))      return 970; 
	if (startswith(i, "vsxseg7bu."))     return 971; 
	if (startswith(i, "vsxseg7e."))      return 972; 
	if (startswith(i, "vsxseg7h."))      return 973; 
	if (startswith(i, "vsxseg7hu."))     return 974; 
	if (startswith(i, "vsxseg7w."))      return 975; 
	if (startswith(i, "vsxseg7wu."))     return 976; 
	if (startswith(i, "vsxseg8b."))      return 977; 
	if (startswith(i, "vsxseg8bu."))     return 978; 
	if (startswith(i, "vsxseg8e."))      return 979; 
	if (startswith(i, "vsxseg8h."))      return 980; 
	if (startswith(i, "vsxseg8hu."))     return 981; 
	if (startswith(i, "vsxseg8w."))      return 982; 
	if (startswith(i, "vsxseg8wu."))     return 983; 
	return 999;
}
