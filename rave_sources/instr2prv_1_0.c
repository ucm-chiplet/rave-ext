static int instr2prv(unsigned int instr){
#define OPV 0x57 
#define LOADFP 0x07
#define STOREFP 0x27 
#define AMO 0x30
	unsigned int rs1 = (instr>>15)&0x1F;
	unsigned int rs2 = (instr>>20)&0x1F;
	//unsigned int dst = (instr>>7)&0x1F;
	unsigned int major = (instr)&0x7F;
	//unsigned int masked = ((instr>>25)&0x01)?0:1;
	unsigned int funct3=(instr>>12)&0x7;
	unsigned int mop;
	//unsigned int mew;
	unsigned int nf;

	switch(major){
		case LOADFP: //funct3 is then width
			mop=(instr>>26)&0x3;
			//mew=(instr>>28)&0x1;
			nf=(instr>>29)&0x7;
			switch(mop){
				case 0: //Unit stride //rs2 is then lumop
					switch(funct3){
						case 0: //Unit-stride
							switch(rs2){
								case 0:
									switch(nf){
										case 0:
											return 418; //vle8.v
										case 1:
											return 446; //vlseg2e8.v
										case 2:
											return 454; //vlseg3e8.v
										case 3:
											return 462; //vlseg4e8.v
										case 4:
											return 470; //vlseg5e8.v
										case 5:
											return 478; //vlseg6e8.v
										case 6:
											return 486; //vlseg7e8.v
										case 7:
											return 494; //vlseg8e8.v
									}break;
								case 8: //Whole register
									switch(nf){
										case 0:
											return 421; //vl1r.v
										case 1:
											return 422; //vl2r.v
										case 3:
											return 423; //vl4r.v
										case 7:
											return 424; //vl8r.v
									} break;
								case 11: //mask load
									return 420; //vlm.v
								case 16:
									switch(nf){
										case 0:
											return 425; //vle8ff.v
										case 1:
											return 445; //vlseg2e8ff.v
										case 2:
											return 453; //vlseg3e8ff.v
										case 3:
											return 461; //vlseg4e8ff.v
										case 4:
											return 469; //vlseg5e8ff.v
										case 5:
											return 477; //vlseg6e8ff.v
										case 6:
											return 485; //vlseg7e8ff.v
										case 7:
											return 493; //vlseg8e8ff.v
									}break;
							}; break;
						case 5:
							switch(rs2){
								case 0:
									switch(nf){
										case 0:
											return 407; //vle16.v
										case 1:
											return 448; //vlseg2e16.v
										case 2:
											return 456; //vlseg3e16.v
										case 3:
											return 464; //vlseg4e16.v
										case 4:
											return 472; //vlseg5e16.v
										case 5:
											return 480; //vlseg6e16.v
										case 6:
											return 488; //vlseg7e16.v
										case 7:
											return 496; //vlseg8e16.v
									}break;
								case 8: //Whole register
									switch(nf){
										case 0:
											return 429; //vl1re16.v
										case 1:
											return 433; //vl2re16.v
										case 3:
											return 437; //vl4re16.v
										case 7:
											return 441; //vl8re16.v
									} break;
								case 16:
									switch(nf){
										case 0:
											return 426; //vle16ff.v
										case 1:
											return 447; //vlseg2e16ff.v
										case 2:
											return 455; //vlseg3e16ff.v
										case 3:
											return 463; //vlseg4e16ff.v
										case 4:
											return 471; //vlseg5e16ff.v
										case 5:
											return 479; //vlseg6e16ff.v
										case 6:
											return 487; //vlseg7e16ff.v
										case 7:
											return 495; //vlseg8e16ff.v
									}break;
							}; break;
						case 6:
							switch(rs2){
								case 0:
									switch(nf){
										case 0:
											return 410; //vle32.v
										case 1:
											return 450; //vlseg2e32.v
										case 2:
											return 458; //vlseg3e32.v
										case 3:
											return 466; //vlseg4e32.v
										case 4:
											return 474; //vlseg5e32.v
										case 5:
											return 482; //vlseg6e32.v
										case 6:
											return 490; //vlseg7e32.v
										case 7:
											return 498; //vlseg8e32.v
									}break;
								case 8: //Whole register
									switch(nf){
										case 0:
											return 430; //vl1re32.v
										case 1:
											return 434; //vl2re32.v
										case 3:
											return 438; //vl4re32.v
										case 7:
											return 442; //vl8re32.v
									} break;
								case 16:
									switch(nf){
										case 0:
											return 427; //vle32ff.v
										case 1:
											return 449; //vlseg2e32ff.v
										case 2:
											return 457; //vlseg3e32ff.v
										case 3:
											return 465; //vlseg4e32ff.v
										case 4:
											return 473; //vlseg5e32ff.v
										case 5:
											return 481; //vlseg6e32ff.v
										case 6:
											return 489; //vlseg7e32ff.v
										case 7:
											return 497; //vlseg8e32ff.v
									}break;
							}; break;
						case 7:
							switch(rs2){
								case 0:
									switch(nf){
										case 0:
											return 409; //vle64.v
										case 1:
											return 452; //vlseg2e64.v
										case 2:
											return 460; //vlseg3e64.v
										case 3:
											return 468; //vlseg4e64.v
										case 4:
											return 476; //vlseg5e64.v
										case 5:
											return 484; //vlseg6e64.v
										case 6:
											return 492; //vlseg7e64.v
										case 7:
											return 500; //vlseg8e64.v
									}break;
								case 8: //Whole register
									switch(nf){
										case 0:
											return 431; //vl1re64.v
										case 1:
											return 435; //vl2re64.v
										case 3:
											return 439; //vl4re64.v
										case 7:
											return 443; //vl8re64.v
									} break;
								case 16:
									switch(nf){
										case 0:
											return 428; //vle64ff.v
										case 1:
											return 451; //vlseg2e64ff.v
										case 2:
											return 459; //vlseg3e64ff.v
										case 3:
											return 467; //vlseg4e64ff.v
										case 4:
											return 475; //vlseg5e64ff.v
										case 5:
											return 483; //vlseg6e64ff.v
										case 6:
											return 491; //vlseg7e64ff.v
										case 7:
											return 499; //vlseg8e64ff.v
									}break;
							}; break;
					} break;
				case 1:  //Indexed-unordered
					switch(funct3){
						case 0:
							switch(nf){
								case 0:
									return 401; //vluxei8.v
								case 1:
									return 529; //vluxseg2ei8.v
								case 2:
									return 533; //vluxseg3ei8.v
								case 3:
									return 537; //vluxseg4ei8.v
								case 4:
									return 541; //vluxseg5ei8.v
								case 5:
									return 545; //vluxseg6ei8.v
								case 6:
									return 549; //vluxseg7ei8.v
								case 7:
									return 553; //vluxseg8ei8.v
							}break;
						case 5:
							switch(nf){
								case 0:
									return 405; //vluxei16.v
								case 1:
									return 530; //vluxseg2ei16.v
								case 2:
									return 534; //vluxseg3ei16.v
								case 3:
									return 538; //vluxseg4ei16.v
								case 4:
									return 538; //vluxseg5ei16.v
								case 5:
									return 546; //vluxseg6ei16.v
								case 6:
									return 550; //vluxseg7ei16.v
								case 7:
									return 554; //vluxseg8ei16.v
							}break;
						case 6:
							switch(nf){
								case 0:
									return 402; //vluxei32.v
								case 1:
									return 531; //vluxseg2ei32.v
								case 2:
									return 535; //vluxseg3ei32.v
								case 3:
									return 539; //vluxseg4ei32.v
								case 4:
									return 543; //vluxseg5ei32.v
								case 5:
									return 547; //vluxseg6ei32.v
								case 6:
									return 551; //vluxseg7ei32.v
								case 7:
									return 555; //vluxseg8ei32.v
							}break;
						case 7:
							switch(nf){
								case 0:
									return 400; //vluxei64.v
								case 1:
									return 532; //vluxseg2ei64.v
								case 2:
									return 536; //vluxseg3ei64.v
								case 3:
									return 540; //vluxseg4ei64.v
								case 4:
									return 544; //vluxseg5ei64.v
								case 5:
									return 548; //vluxseg6ei64.v
								case 6:
									return 552; //vluxseg7ei64.v
								case 7:
									return 556; //vluxseg8ei64.v
							}break;
					} break;
				case 2: //Strided
					switch(funct3){
						case 0:
							switch(nf){
								case 0:
									return 403; //vlse8.v
								case 1:
									return 501; //vlsseg2e8.v
								case 2:
									return 505; //vlsseg3e8.v
								case 3:
									return 509; //vlsseg4e8.v
								case 4:
									return 513; //vlsseg5e8.v
								case 5:
									return 517; //vlsseg6e8.v
								case 6:
									return 521; //vlsseg7e8.v
								case 7:
									return 525; //vlsseg8e8.v
							}break;
						case 5:
							switch(nf){
								case 0:
									return 414; //vlse16.v
								case 1:
									return 502; //vlsseg2e16.v
								case 2:
									return 506; //vlsseg3e16.v
								case 3:
									return 510; //vlsseg4e16.v
								case 4:
									return 514; //vlsseg5e16.v
								case 5:
									return 518; //vlsseg6e16.v
								case 6:
									return 522; //vlsseg7e16.v
								case 7:
									return 526; //vlsseg8e16.v
							}break;
						case 6:
							switch(nf){
								case 0:
									return 417; //vlse32.v
								case 1:
									return 503; //vlsseg2e32.v
								case 2:
									return 507; //vlsseg3e32.v
								case 3:
									return 511; //vlsseg4e32.v
								case 4:
									return 515; //vlsseg5e32.v
								case 5:
									return 519; //vlsseg6e32.v
								case 6:
									return 523; //vlsseg7e32.v
								case 7:
									return 527; //vlsseg8e32.v
							}break;
						case 7:
							switch(nf){
								case 0:
									return 406; //vlse64.v
								case 1:
									return 504; //vlsseg2e64.v
								case 2:
									return 508; //vlsseg3e64.v
								case 3:
									return 512; //vlsseg4e64.v
								case 4:
									return 516; //vlsseg5e64.v
								case 5:
									return 520; //vlsseg6e64.v
								case 6:
									return 524; //vlsseg7e64.v
								case 7:
									return 528; //vlsseg8e64.v
							}break;
					} break;
				case 3: //Indexed-ordered 
					switch(funct3){
						case 0:
							switch(nf){
								case 0:
									return 419; //vloxei8.v
								case 1:
									return 557; //vloxseg2ei8.v
								case 2:
									return 561; //vloxseg3ei8.v
								case 3:
									return 565; //vloxseg4ei8.v
								case 4:
									return 569; //vloxseg5ei8.v
								case 5:
									return 573; //vloxseg6ei8.v
								case 6:
									return 577; //vloxseg7ei8.v
								case 7:
									return 581; //vloxseg8ei8.v
							}break;
						case 5:
							switch(nf){
								case 0:
									return 416; //vloxei16.v
								case 1:
									return 558; //vloxseg2ei16.v
								case 2:
									return 562; //vloxseg3ei16.v
								case 3:
									return 566; //vloxseg4ei16.v
								case 4:
									return 570; //vloxseg5ei16.v
								case 5:
									return 574; //vloxseg6ei16.v
								case 6:
									return 578; //vloxseg7ei16.v
								case 7:
									return 582; //vloxseg8ei16.v
							}break;
						case 6:
							switch(nf){
								case 0:
									return 404; //vloxei32.v
								case 1:
									return 559; //vloxseg2ei32.v
								case 2:
									return 563; //vloxseg3ei32.v
								case 3:
									return 567; //vloxseg4ei32.v
								case 4:
									return 571; //vloxseg5ei32.v
								case 5:
									return 575; //vloxseg6ei32.v
								case 6:
									return 579; //vloxseg7ei32.v
								case 7:
									return 583; //vloxseg8ei32.v
							}break;
						case 7:
							switch(nf){
								case 0:
									return 408; //vloxei64.v
								case 1:
									return 560; //vloxseg2ei64.v
								case 2:
									return 564; //vloxseg3ei64.v
								case 3:
									return 568; //vloxseg4ei64.v
								case 4:
									return 572; //vloxseg5ei64.v
								case 5:
									return 576; //vloxseg6ei64.v
								case 6:
									return 580; //vloxseg7ei64.v
								case 7:
									return 584; //vloxseg8ei64.v
							}break;
					} break;
			}
			break;
		case STOREFP: //funct3 is then width
			mop=(instr>>26)&0x7;
			nf=(instr>>29)&0x7;
			switch(mop){
				case 0:  //unit-stride //rs2 is then sumop
					switch(funct3){
						case 0:
							switch(rs2){
								case 0:
									switch(nf){
										case 0:
											return 809; //vse8.v
										case 1:
											return 827; //vsseg2e8.v
										case 2:
											return 831; //vsseg3e8.v
										case 3:
											return 835; //vsseg4e8.v
										case 4:
											return 839; //vsseg5e8.v
										case 5:
											return 843; //vsseg6e8.v
										case 6:
											return 847; //vsseg7e8.v
										case 7:
											return 851; //vsseg8e8.v
									}break;
								case 8: //Whole register
									switch(nf){
										case 0:
											return 823; //vs1r.v
										case 1:
											return 824; //vs2r.v
										case 3:
											return 825; //vs4r.v
										case 7:
											return 826; //vs8r.v
									} break;
								case 11: //mask load
									return 820; //vsm.v
							} break;
						case 5:
							switch(nf){
								case 0:
									return 806; //vse16.v
								case 1:
									return 828; //vsseg2e16.v
								case 2:
									return 832; //vsseg3e16.v
								case 3:
									return 836; //vsseg4e16.v
								case 4:
									return 840; //vsseg5e16.v
								case 5:
									return 844; //vsseg6e16.v
								case 6:
									return 848; //vsseg7e16.v
								case 7:
									return 852; //vsseg8e16.v
							}break;
						case 6:
							switch(nf){
								case 0:
									return 817; //vse32.v
								case 1:
									return 829; //vsseg2e32.v
								case 2:
									return 833; //vsseg3e32.v
								case 3:
									return 837; //vsseg4e32.v
								case 4:
									return 841; //vsseg5e32.v
								case 5:
									return 845; //vsseg6e32.v
								case 6:
									return 849; //vsseg7e32.v
								case 7:
									return 853; //vsseg8e32.v
							}break;
						case 7:
							switch(nf){
								case 0:
									return 813; //vse64.v
								case 1:
									return 830; //vsseg2e64.v
								case 2:
									return 834; //vsseg3e64.v
								case 3:
									return 838; //vsseg4e64.v
								case 4:
									return 842; //vsseg5e64.v
								case 5:
									return 846; //vsseg6e64.v
								case 6:
									return 850; //vsseg7e64.v
								case 7:
									return 854; //vsseg8e64.v
							}break;
					} break;
				case 1: //indexed-unordered
					switch(funct3){
						case 0:
							switch(nf){
								case 0:
									return 800; //vsuxei8.v
								case 1:
									return 883; //vsuxseg2ei8.v
								case 2:
									return 887; //vsuxseg3ei8.v
								case 3:
									return 891; //vsuxseg4ei8.v
								case 4:
									return 895; //vsuxseg5ei8.v
								case 5:
									return 899; //vsuxseg6ei8.v
								case 6:
									return 903; //vsuxseg7ei8.v
								case 7:
									return 907; //vsuxseg8ei8.v
							}break;
						case 5:
							switch(nf){
								case 0:
									return 807; //vsuxei16.v
								case 1:
									return 884; //vsuxseg2ei16.v
								case 2:
									return 888; //vsuxseg3ei16.v
								case 3:
									return 892; //vsuxseg4ei16.v
								case 4:
									return 896; //vsuxseg5ei16.v
								case 5:
									return 900; //vsuxseg6ei16.v
								case 6:
									return 904; //vsuxseg7ei16.v
								case 7:
									return 908; //vsuxseg8ei16.v
							}break;
						case 6:
							switch(nf){
								case 0:
									return 821; //vsuxei32.v
								case 1:
									return 885; //vsuxseg2ei32.v
								case 2:
									return 889; //vsuxseg3ei32.v
								case 3:
									return 893; //vsuxseg4ei32.v
								case 4:
									return 897; //vsuxseg5ei32.v
								case 5:
									return 901; //vsuxseg6ei32.v
								case 6:
									return 905; //vsuxseg7ei32.v
								case 7:
									return 909; //vsuxseg8ei32.v
							}break;
						case 7:
							switch(nf){
								case 0:
									return 808; //vsuxei64.v
								case 1:
									return 886; //vsuxseg2ei64.v
								case 2:
									return 890; //vsuxseg3ei64.v
								case 3:
									return 894; //vsuxseg4ei64.v
								case 4:
									return 898; //vsuxseg5ei64.v
								case 5:
									return 902; //vsuxseg6ei64.v
								case 6:
									return 906; //vsuxseg7ei64.v
								case 7:
									return 910; //vsuxseg8ei64.v
							}break;
					} break;
				case 2: //strided
					switch(funct3){
						case 0:
							switch(nf){
								case 0:
									return 801; //vsse8.v
								case 1:
									return 855; //vssseg2e8.v
								case 2:
									return 859; //vssseg3e8.v
								case 3:
									return 863; //vssseg4e8.v
								case 4:
									return 867; //vssseg5e8.v
								case 5:
									return 871; //vssseg6e8.v
								case 6:
									return 871; //vssseg7e8.v
								case 7:
									return 879; //vssseg8e8.v
							}break;
						case 5:
							switch(nf){
								case 0:
									return 812; //vsse16.v
								case 1:
									return 856; //vssseg2e16.v
								case 2:
									return 860; //vssseg3e16.v
								case 3:
									return 864; //vssseg4e16.v
								case 4:
									return 868; //vssseg5e16.v
								case 5:
									return 872; //vssseg6e16.v
								case 6:
									return 876; //vssseg7e16.v
								case 7:
									return 880; //vssseg8e16.v
							}break;
						case 6:
							switch(nf){
								case 0:
									return 814; //vsse32.v
								case 1:
									return 857; //vssseg2e32.v
								case 2:
									return 861; //vssseg3e32.v
								case 3:
									return 865; //vssseg4e32.v
								case 4:
									return 869; //vssseg5e32.v
								case 5:
									return 873; //vssseg6e32.v
								case 6:
									return 877; //vssseg7e32.v
								case 7:
									return 881; //vssseg8e32.v
							}break;
						case 7:
							switch(nf){
								case 0:
									return 804; //vsse64.v
								case 1:
									return 858; //vssseg2e64.v
								case 2:
									return 862; //vssseg3e64.v
								case 3:
									return 866; //vssseg4e64.v
								case 4:
									return 870; //vssseg5e64.v
								case 5:
									return 874; //vssseg6e64.v
								case 6:
									return 878; //vssseg7e64.v
								case 7:
									return 882; //vssseg8e64.v
							}break;
					} break;
				case 3: //indexed-ordered
					switch(funct3){
						case 0:
							switch(nf){
								case 0:
									return 802; //vsoxei8.v
								case 1:
									return 911; //vsoxseg2ei8.v
								case 2:
									return 915; //vsoxseg3ei8.v
								case 3:
									return 919; //vsoxseg4ei8.v
								case 4:
									return 923; //vsoxseg5ei8.v
								case 5:
									return 927; //vsoxseg6ei8.v
								case 6:
									return 931; //vsoxseg7ei8.v
								case 7:
									return 935; //vsoxseg8ei8.v
							}break;
						case 5:
							switch(nf){
								case 0:
									return 803; //vsoxei16.v
								case 1:
									return 912; //vsoxseg2ei16.v
								case 2:
									return 916; //vsoxseg3ei16.v
								case 3:
									return 920; //vsoxseg4ei16.v
								case 4:
									return 924; //vsoxseg5ei16.v
								case 5:
									return 928; //vsoxseg6ei16.v
								case 6:
									return 932; //vsoxseg7ei16.v
								case 7:
									return 936; //vsoxseg8ei16.v
							}break;
						case 6:
							switch(nf){
								case 0:
									return 811; //vsoxei32.v
								case 1:
									return 913; //vsoxseg2ei32.v
								case 2:
									return 917; //vsoxseg3ei32.v
								case 3:
									return 921; //vsoxseg4ei32.v
								case 4:
									return 925; //vsoxseg5ei32.v
								case 5:
									return 929; //vsoxseg6ei32.v
								case 6:
									return 933; //vsoxseg7ei32.v
								case 7:
									return 937; //vsoxseg8ei32.v
							}break;
						case 7:
							switch(nf){
								case 0:
									return 815; //vsoxei64.v
								case 1:
									return 914; //vsoxseg2ei64.v
								case 2:
									return 918; //vsoxseg3ei64.v
								case 3:
									return 922; //vsoxseg4ei64.v
								case 4:
									return 926; //vsoxseg5ei64.v
								case 5:
									return 930; //vsoxseg6ei64.v
								case 6:
									return 934; //vsoxseg7ei64.v
								case 7:
									return 938; //vsoxseg8ei64.v
							}break;
					} break;
			}
			break;
		case AMO:
			break;
		case OPV:
			if ((((instr>>25) & 0x7F)==64) && (((instr>>12)&0x7)==7)){ //is vsetvl
				return 1001; //vsetvl
				break;
			}else if ((((instr>>31) & 0x1)==0) && (((instr>>12)&0x7)==7)){ //is vsetvli
				//unsigned int sew = 1 << (((instr>>22)&0x7)+3);
				//unsigned int lmul = (((instr>>20)&0x3)+1);
				return 1002; //vsetvli
				break;
			}
			unsigned int funct6=(instr>>26)&0x3F;
			switch(funct6){
				case 0:
					switch(funct3){
						case 0: //OPIVV
							return 1; //vadd.vv
						case 4: //OPIVX
							return 1; //vadd.vx
						case 3: //OPIVI
							return 1; //vadd.vi
						case 2: //OPMVV
							return 25; //vredsum.vs
						case 1: //OPFVV
							return 100; //vfadd.vv
						case 5: //OPFVF
							return 100; //vfadd.vf
					} break;
				case 1:
					switch(funct3){
						case 2: //OPMVV
							return 26; //vredand.vs
						case 1: //OPFVV
							return 106; //vfredusum.vs
					} break;
				case 2:
					switch(funct3){
						case 0: //OPIVV
							return 2; //vsub.vv
						case 4: //OPIVX
							return 2; //vsub.vx
						case 2: //OPMVV
							return 27; //vredor.vs
						case 1: //OPFVV
							return 107; //vfsub.vv
						case 5: //OPFVF
							return 107; //vfsub.vf
					} break;
				case 3:
					switch(funct3){
						case 4: //OPIVX
							return 21; //vneg.v
							return 3; //vrsub.vx
							break;
						case 3: //OPIVI
							return 3; //vrsub.vi
						case 2: //OPMVV
							return 28; //vredxor.vs
						case 1: //OPFVV
							return 113; //vfredosum.vs
					} break;
				case 4:
					switch(funct3){
						case 0: //OPIVV
							return 36; //vminu.vv
						case 4: //OPIVX
							return 36; //vminu.vx
						case 2: //OPMVV
							return 30; //vredminu.vs
						case 1: //OPFVV
							return 114; //vfmin.vv
						case 5: //OPFVF
							return 114; //vfmin.vf
					} break;
				case 5:
					switch(funct3){
						case 0: //OPIVV
							return 35; //vmin.vv
						case 4: //OPIVX
							return 35; //vmin.vx
						case 2: //OPMVV
							return 29; //vredmin.vs
						case 1: //OPFVV
							return 115; //vfredmin.vs
					} break;
				case 6:
					switch(funct3){
						case 0: //OPIVV
							return 38; //vmaxu.vv
						case 4: //OPIVX
							return 38; //vmaxu.vx
						case 2: //OPMVV
							return 32; //vredmaxu.vs
						case 1: //OPFVV
							return 116; //vfmax.vv
						case 5: //OPFVF
							return 116; //vfmax.vf
					} break;
				case 7:
					switch(funct3){
						case 0: //OPIVV
							return 37; //vmax.vv
						case 4: //OPIVX
							return 37; //vmax.vx
						case 2: //OPMVV
							return 31; //vredmax.vs
						case 1: //OPFVV
							return 117; //vfredmax.vs
					} break;
				case 8:
					switch(funct3){
						case 1: //OPFVV
							return 118; //vfsgnj.vv
						case 5: //OPFVF
							return 118; //vfsgnj.vf
						case 2: //OPMVV
							return 53; //vaaddu.vv
						case 6: //OPMVX
							return 53; //vaaddu.vx
					} break;
				case 9:
					switch(funct3){
						case 0: //OPIVV
							return 17; //vand.vv
						case 4: //OPIVX
							return 17; //vand.vx
						case 3: //OPIVI
							return 17; //vand.vi
						case 1: //OPFVV
							return 138; //vfneg.v
							return 110; //vfsgnjn.vv
							break;
						case 5: //OPFVF
							return 110; //vfsgnjn.vf
						case 2: //OPMVV
							return 51; //vaadd.vv
						case 6: //OPMVX
							return 51; //vaadd.vx
					} break;
				case 10:
					switch(funct3){
						case 0: //OPIVV
							return 18; //vor.vv
						case 4: //OPIVX
							return 18; //vor.vx
						case 3: //OPIVI
							return 18; //vor.vi
						case 1: //OPFVV
							return 139; //vfabs.v
							return 119; //vfsgnjx.vv
							break;
						case 5: //OPFVF
							return 119; //vfsgnjx.vf
						case 2: //OPMVV
							return 54; //vasubu.vv
						case 6: //OPMVX
							return 54; //vasubu.vx
					} break;
				case 11:
					switch(funct3){
						case 0: //OPIVV
							return 19; //vxor.vv
						case 4: //OPIVX
							return 19; //vxor.vx
						case 3: //OPIVI
							return 20; //vnot.v
							return 19; //vxor.vi
							break;
						case 2: //OPMVV
							return 52; //vasub.vv
						case 6: //OPMVX
							return 52; //vasub.vx
					} break;
				case 12:
					switch(funct3){
						case 0: //OPIVV
							return 203; //vrgather.vv
						case 4: //OPIVX
							return 203; //vrgather.vx
						case 3: //OPIVI
							return 203; //vrgather.vi
						case 1: //OPFVV
							return 202; //vfmv.f.s
					} break;
				case 14:
					switch(funct3){
						case 0: //OPIVV
							return 204; //vrgatherei16.vv
						case 4: //OPIVX
							return 222; //vslideup.vx
						case 3: //OPIVI
							return 222; //vslideup.vi
						case 6: //OPMVX
							return 223; //vslide1up.vx
						case 5: //OPFVF
							return 224; //vfslide1up.vf
					} break;
				case 15:
					switch(funct3){
						case 4: //OPIVX
							return 220; //vslidedown.vx
						case 3: //OPIVI
							return 220; //vslidedown.vi
						case 6: //OPMVX
							return 221; //vslide1down.vx
						case 5: //OPFVF
							return 225; //vfslide1down.vf
					} break;
				case 16:
					switch(funct3){
						case 0: //OPIVV
							return 40; //vadc.vvm
						case 4: //OPIVX
							return 40; //vadc.vxm
						case 3: //OPIVI
							return 40; //vadc.vim
						case 2: //OPMVV
							switch(rs1){
								case 0:
									return 201; //vmv.x.s
								case 16:
									return 231; //vcpop.m
								case 17:
									return 232; //vfirst.m
							} break;
						case 1: //OPFVV
							return 202; //vfmv.f.s
						case 5: //OPFVF
							return 202; //vfmv.s.f
						case 6: //OPMVX
							return 201; //vmv.s.x
					} break;
				case 17:
					switch(funct3){
						case 0: //OPIVV
							return 41; //vmadc.vvm
							return 41; //vmadc.vv
							break;
						case 4: //OPIVX
							return 41; //vmadc.vxm
							return 41; //vmadc.vx
							break;
						case 3: //OPIVI
							return 41; //vmadc.vim
							return 41; //vmadc.vi
							break;
					} break;
				case 18:
					switch(funct3){
						case 0: //OPIVV
							return 42; //vsbc.vvm
						case 4: //OPIVX
							return 42; //vsbc.vxm
						case 1: //OPFVV
							switch(rs1){
								case 0:
									return 242; //vfcvt.xu.f.v
								case 1:
									return 242; //vfcvt.x.f.v
								case 2:
									return 242; //vfcvt.f.xu.v
								case 3:
									return 242; //vfcvt.f.x.v
								case 6:
									return 242; //vfcvt.rtz.xu.f.v
								case 7:
									return 242; //vfcvt.rtz.x.f.v
								case 8:
									return 243; //vfwcvt.xu.f.v
								case 9:
									return 243; //vfwcvt.x.f.v
								case 10:
									return 243; //vfwcvt.f.xu.v
								case 11:
									return 243; //vfwcvt.f.x.v
								case 12:
									return 243; //vfwcvt.f.f.v
								case 14:
									return 243; //vfwcvt.rtz.xu.f.v
								case 15:
									return 243; //vfwcvt.rtz.x.f.v
								case 16:
									return 244; //vfncvt.xu.f.w
								case 17:
									return 244; //vfncvt.x.f.w
								case 18:
									return 244; //vfncvt.f.xu.w
								case 19:
									return 244; //vfncvt.f.x.w
								case 20:
									return 244; //vfncvt.f.f.w
								case 21:
									return 244; //vfncvt.rod.f.f.w
								case 22:
									return 244; //vfncvt.rtz.xu.f.w
								case 23:
									return 244; //vfncvt.rtz.x.f.w
							}break;
						case 2: //OPMVV
							switch(rs1){
								case 2:
									return 234; //vzext.vf8
								case 3:
									return 235; //vsext.vf8
								case 4:
									return 234; //vzext.vf4
								case 5:
									return 235; //vsext.vf4
								case 6:
									return 234; //vzext.vf2
								case 7:
									return 235; //vsext.vf2
							} break;
					} break;
				case 19:
					switch(funct3){
						case 0: //OPIVV
							return 43; //vmsbc.vvm
							return 43; //vmsbc.vv
							break;
						case 4: //OPIVX
							return 43; //vmsbc.vxm
							return 43; //vmsbc.vx
							break;
						case 1: //OPFVV
							switch(rs1){
								case 0:
									return 109; //vfsqrt.v
								case 4:
									return 141; //vfrsqrt7.v
								case 5:
									return 140; //vfrec7.v
								case 16:
									return 247; //vfclass.v
							}break;
					} break;
				case 20:
					switch(funct3){
						case 2: //OPMVV
							switch(rs1){
								case 1:
									return 250; //vmsbf.m
								case 2:
									return 251; //vmsof.m
								case 3:
									return 252; //vmsif.m
								case 16:
									return 253; //viota.m
								case 17:
									return 200; //vid.v
							}break;
					} break;
				case 22:
					switch(funct3){
						case 2: //OPMVV
							switch(rs1){
								case 1:
									return 250; //vmsbf.m
								case 2:
									return 251; //vmsof.m
								case 3:
									return 252; //vmsif.m
								case 16:
									return 253; //viota.m
								case 17:
									return 200; //vid.v
							}break;
					} break;
				case 23:
					switch(funct3){
						case 0: //OPIVV
							return 201; //vmv.v.v
							return 210; //vmerge.vvm
						case 4: //OPIVX
							return 201; //vmv.v.x
							return 210; //vmerge.vxm
						case 3: //OPIVI
							return 201; //vmv.v.i
							return 210; //vmerge.vim
						case 2: //OPMVV
							return 233; //vcompress.vm
						case 5: //OPFVF
							return 202; //vfmv.v.f
							return 211; //vfmerge.vfm
					} break;
				case 24:
					switch(funct3){
						case 0: //OPIVV
							return 300; //vmseq.vv
						case 4: //OPIVX
							return 300; //vmseq.vx
						case 3: //OPIVI
							return 300; //vmseq.vi
						case 2: //OPMVV
							return 318; //vmandn.mm
						case 1: //OPFVV
							return 310; //vmfeq.vv
						case 5: //OPFVF
							return 310; //vmfeq.vf
					} break;
				case 25:
					switch(funct3){
						case 0: //OPIVV
							return 301; //vmsne.vv
						case 4: //OPIVX
							return 301; //vmsne.vx
						case 3: //OPIVI
							return 301; //vmsne.vi
						case 2: //OPMVV
							return 326; //vmmv.m
							return 317; //vmand.mm
							break;
						case 1: //OPFVV
							return 312; //vmfle.vv
						case 5: //OPFVF
							return 312; //vmfle.vf
					} break;
				case 26:
					switch(funct3){
						case 0: //OPIVV
							return 304; //vmsltu.vv
						case 4: //OPIVX
							return 304; //vmsltu.vx
						case 2: //OPMVV
							return 320; //vmor.mm
					} break;
				case 27:
					switch(funct3){
						case 0: //OPIVV
							return 303; //vmslt.vv
						case 4: //OPIVX
							return 303; //vmslt.vx
						case 2: //OPMVV
							return 325; //vmclr.m
							return 323; //vmxor.mm
							break;
						case 1: //OPFVV
							return 313; //vmflt.vv
						case 5: //OPFVF
							return 313; //vmflt.vf
					} break;
				case 28:
					switch(funct3){
						case 0: //OPIVV
							return 305; //vmsleu.vv
						case 4: //OPIVX
							return 305; //vmsleu.vx
						case 3: //OPIVI
							return 305; //vmsleu.vi
						case 2: //OPMVV
							return 321; //vmorn.mm
						case 1: //OPFVV
							return 311; //vmfne.vv
						case 5: //OPFVF
							return 311; //vmfne.vf
					} break;
				case 29:
					switch(funct3){
						case 0: //OPIVV
							return 302; //vmsle.vv
						case 4: //OPIVX
							return 302; //vmsle.vx
						case 3: //OPIVI
							return 302; //vmsle.vi
						case 2: //OPMVV
							return 316; //vmnot.m
							return 319; //vmnand.mm
							break;
						case 5: //OPFVF
							return 314; //vmfgt.vf
					} break;
				case 30:
					switch(funct3){
						case 4: //OPIVX
							return 307; //vmsgtu.vx
						case 3: //OPIVI
							return 307; //vmsgtu.vi
						case 2: //OPMVV
							return 322; //vmnor.mm
					} break;
				case 31:
					switch(funct3){
						case 4: //OPIVX
							return 306; //vmsgt.vx
						case 3: //OPIVI
							return 306; //vmsgt.vi
						case 2: //OPMVV
							return 327; //vmset.m
							return 324; //vmxnor.mm
							break;
						case 5: //OPFVF
							return 315; //vmfge.vf
					} break;
				case 32:
					switch(funct3){
						case 0: //OPIVV
							return 45; //vsaddu.vv
						case 4: //OPIVX
							return 45; //vsaddu.vx
						case 3: //OPIVI
							return 45; //vsaddu.vi
						case 2: //OPMVV
							return 11; //vdivu.vv
						case 6: //OPMVX
							return 11; //vdivu.vx
						case 1: //OPFVV
							return 112; //vfdiv.vv
						case 5: //OPFVF
							return 112; //vfdiv.vf
					} break;
				case 33:
					switch(funct3){
						case 0: //OPIVV
							return 44; //vsadd.vv
						case 4: //OPIVX
							return 44; //vsadd.vx
						case 3: //OPIVI
							return 44; //vsadd.vi
						case 2: //OPMVV
							return 10; //vdiv.vv
						case 6: //OPMVX
							return 10; //vdiv.vx
						case 5: //OPFVF
							return 120; //vfrdiv.vf
					} break;
				case 34:
					switch(funct3){
						case 0: //OPIVV
							return 47; //vssubu.vv
						case 4: //OPIVX
							return 47; //vssubu.vx
						case 2: //OPMVV
							return 13; //vremu.vv
						case 6: //OPMVX
							return 13; //vremu.vx
					} break;
				case 35:
					switch(funct3){
						case 0: //OPIVV
							return 46; //vssub.vv
						case 4: //OPIVX
							return 46; //vssub.vx
						case 2: //OPMVV
							return 12; //vrem.vv
						case 6: //OPMVX
							return 12; //vrem.vx
					} break;
				case 36:
					switch(funct3){
						case 2: //OPMVV
							return 6; //vmulhu.vv
						case 6: //OPMVX
							return 6; //vmulhu.vx
						case 1: //OPFVV
							return 105; //vfmul.vv
						case 5: //OPFVF
							return 105; //vfmul.vf
					} break;
				case 37:
					switch(funct3){
						case 0: //OPIVV
							return 14; //vsll.vv
						case 4: //OPIVX
							return 14; //vsll.vx
						case 3: //OPIVI
							return 14; //vsll.vi
						case 2: //OPMVV
							return 4; //vmul.vv
						case 6: //OPMVX
							return 4; //vmul.vx
					} break;
				case 38:
					switch(funct3){
						case 2: //OPMVV
							return 7; //vmulhsu.vv
						case 6: //OPMVX
							return 7; //vmulhsu.vx
					} break;
				case 39:
					switch(funct3){
						case 0: //OPIVV
							return 50; //vsmul.vv
						case 4: //OPIVX
							return 50; //vsmul.vx
						case 2: //OPMVV
							return 5; //vmulh.vv
						case 6: //OPMVX
							return 5; //vmulh.vx
						case 5: //OPFVF
							return 135; //vfrsub.vf
						case 3: //OPIVI
							switch(rs1){
								case 0:
									return 205; //vmv1r.v
								case 1:
									return 206; //vmv2r.v
								case 3:
									return 207; //vmv4r.v
								case 7:
									return 208; //vmv8r.v
							} break;
					} break;
				case 40:
					switch(funct3){
						case 0: //OPIVV
							return 16; //vsrl.vv
						case 4: //OPIVX
							return 16; //vsrl.vx
						case 3: //OPIVI
							return 16; //vsrl.vi
						case 1: //OPFVV
							return 101; //vfmadd.vv
						case 5: //OPFVF
							return 101; //vfmadd.vf
					} break;
				case 41:
					switch(funct3){
						case 0: //OPIVV
							return 15; //vsra.vv
						case 4: //OPIVX
							return 15; //vsra.vx
						case 3: //OPIVI
							return 15; //vsra.vi
						case 2: //OPMVV
							return 8; //vmadd.vv
						case 6: //OPMVX
							return 8; //vmadd.vx
						case 1: //OPFVV
							return 121; //vfnmadd.vv
						case 5: //OPFVF
							return 121; //vfnmadd.vf
					} break;
				case 42:
					switch(funct3){
						case 0: //OPIVV
							return 48; //vssrl.vv
						case 4: //OPIVX
							return 48; //vssrl.vx
						case 3: //OPIVI
							return 48; //vssrl.vi
						case 1: //OPFVV
							return 102; //vfmsub.vv
						case 5: //OPFVF
							return 102; //vfmsub.vf
					} break;
				case 43:
					switch(funct3){
						case 0: //OPIVV
							return 49; //vssra.vv
						case 4: //OPIVX
							return 49; //vssra.vx
						case 3: //OPIVI
							return 49; //vssra.vi
						case 2: //OPMVV
							return 58; //vnmsub.vv
						case 6: //OPMVX
							return 58; //vnmsub.vx
						case 1: //OPFVV
							return 122; //vfnmsub.vv
						case 5: //OPFVF
							return 122; //vfnmsub.vf
					} break;
				case 44:
					switch(funct3){
						case 0: //OPIVV
							return 55; //vnsrl.wv
						case 4: //OPIVX
							return 55; //vnsrl.wx
						case 3: //OPIVI
							return 55; //vnsrl.wi
						case 1: //OPFVV
							return 108; //vfmacc.vv
						case 5: //OPFVF
							return 108; //vfmacc.vf
					} break;
				case 45:
					switch(funct3){
						case 0: //OPIVV
							return 56; //vnsra.wv
						case 4: //OPIVX
							return 56; //vnsra.wx
						case 3: //OPIVI
							return 56; //vnsra.wi
						case 2: //OPMVV
							return 9; //vmacc.vv
						case 6: //OPMVX
							return 9; //vmacc.vx
						case 1: //OPFVV
							return 123; //vfnmacc.vv
						case 5: //OPFVF
							return 123; //vfnmacc.vf
					} break;
				case 46:
					switch(funct3){
						case 0: //OPIVV
							return 246; //vnclipu.wv
						case 4: //OPIVX
							return 246; //vnclipu.wx
						case 3: //OPIVI
							return 246; //vnclipu.wi
						case 1: //OPFVV
							return 111; //vfmsac.vv
						case 5: //OPFVF
							return 111; //vfmsac.vf
					} break;
				case 47:
					switch(funct3){
						case 0: //OPIVV
							return 245; //vnclip.wv
						case 4: //OPIVX
							return 245; //vnclip.wx
						case 3: //OPIVI
							return 245; //vnclip.wi
						case 2: //OPMVV
							return 57; //vnmsac.vv
						case 6: //OPMVX
							return 57; //vnmsac.vx
						case 1: //OPFVV
							return 124; //vfnmsac.vv
						case 5: //OPFVF
							return 124; //vfnmsac.vf
					} break;
				case 48:
					switch(funct3){
						case 0: //OPIVV
							return 74; //vwredsumu.vs
						case 2: //OPMVV
							return 60; //vwaddu.vv
						case 6: //OPMVX
							return 60; //vwaddu.vx
						case 1: //OPFVV
							return 125; //vfwadd.vv
						case 5: //OPFVF
							return 125; //vfwadd.vf
					} break;
				case 49:
					switch(funct3){
						case 0: //OPIVV
							return 75; //vwredsum.vs
						case 2: //OPMVV
							return 59; //vwadd.vv
						case 6: //OPMVX
							return 59; //vwadd.vx
						case 1: //OPFVV
							return 126; //vfwredusum.vs
					} break;
				case 50:
					switch(funct3){
						case 2: //OPMVV
							return 62; //vwsubu.vv
						case 6: //OPMVX
							return 62; //vwsubu.vx
						case 1: //OPFVV
							return 127; //vfwsub.vv
						case 5: //OPFVF
							return 127; //vfwsub.vf
					} break;
				case 51:
					switch(funct3){
						case 2: //OPMVV
							return 61; //vwsub.vv
						case 6: //OPMVX
							return 61; //vwsub.vx
						case 1: //OPFVV
							return 128; //vfwredosum.vs
					} break;
				case 52:
					switch(funct3){
						case 2: //OPMVV
							return 60; //vwaddu.wv
						case 6: //OPMVX
							return 60; //vwaddu.wx
						case 1: //OPFVV
							return 125; //vfwadd.wv
						case 5: //OPFVF
							return 125; //vfwadd.wf
					} break;
				case 53:
					switch(funct3){
						case 2: //OPMVV
							return 59; //vwadd.wv
						case 6: //OPMVX
							return 59; //vwadd.wx
					} break;
				case 54:
					switch(funct3){
						case 2: //OPMVV
							return 62; //vwsubu.wv
						case 6: //OPMVX
							return 62; //vwsubu.wx
						case 1: //OPFVV
							return 127; //vfwsub.wv
						case 5: //OPFVF
							return 127; //vfwsub.wf
					} break;
				case 55:
					switch(funct3){
						case 2: //OPMVV
							return 61; //vwsub.wv
						case 6: //OPMVX
							return 61; //vwsub.wx
					} break;
				case 56:
					switch(funct3){
						case 2: //OPMVV
							return 63; //vwmulu.vv
						case 6: //OPMVX
							return 63; //vwmulu.vx
						case 1: //OPFVV
							return 129; //vfwmul.vv
						case 5: //OPFVF
							return 129; //vfwmul.vf
					} break;
				case 58:
					switch(funct3){
						case 2: //OPMVV
							return 64; //vwmulsu.vv
						case 6: //OPMVX
							return 64; //vwmulsu.vx
					} break;
				case 59:
					switch(funct3){
						case 2: //OPMVV
							return 65; //vwmul.vv
						case 6: //OPMVX
							return 65; //vwmul.vx
					} break;
				case 60:
					switch(funct3){
						case 2: //OPMVV
							return 67; //vwmaccu.vv
						case 6: //OPMVX
							return 67; //vwmaccu.vx
						case 1: //OPFVV
							return 131; //vfwmacc.vv
						case 5: //OPFVF
							return 131; //vfwmacc.vf
					} break;
				case 61:
					switch(funct3){
						case 2: //OPMVV
							return 66; //vwmacc.vv
						case 6: //OPMVX
							return 66; //vwmacc.vx
						case 1: //OPFVV
							return 132; //vfwnmacc.vv
						case 5: //OPFVF
							return 132; //vfwnmacc.vf
					} break;
				case 62:
					switch(funct3){
						case 2: //OPMVV
							return 69; //vwmaccus.vv
						case 6: //OPMVX
							return 69; //vwmaccus.vx
						case 1: //OPFVV
							return 133; //vfwmsac.vv
						case 5: //OPFVF
							return 133; //vfwmsac.vf
					} break;
				case 63:
					switch(funct3){
						case 6: //OPMVX
							return 68; //vwmaccsu.vx
						case 2: //OPMVV
							return 68; //vwmaccsu.vv
						case 1: //OPFVV
							return 134; //vfwnmsac.vv
						case 5: //OPFVF
							return 134; //vfwnmsac.vf
					} break;
					break;
			}
			break;
	}
	return 999; //illegal
}
