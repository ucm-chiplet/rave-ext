int instr2prv(unsigned int instr){
#define OPV 0x57 
#define LOADFP 0x07
#define STOREFP 0x27 
#define AMO 0x30
	unsigned int rs1 = (instr>>15)&0x1F;
	unsigned int rs2 = (instr>>20)&0x1F;
	unsigned int dst = (instr>>7)&0x1F;
	unsigned int major = (instr)&0x7F;
	unsigned int masked = ((instr>>25)&0x01)?0:1;
	unsigned int funct3=(instr>>12)&0x7;
	unsigned int mop=(instr>>26)&0x7;
	unsigned int nf;
	switch(major){
		case LOADFP:
			nf=(instr>>29)&0x7;
			switch(mop){
				case 0: 
					switch(funct3){
						case 0:
							switch(rs2){
								case 0:
									switch(nf){
										case 0:
											return 404; //vlbu.v
										case 1:
											return 428; //vlseg2bu.v
										case 2:
											return 442; //vlseg3bu.v
										case 3:
											return 456; //vlseg4bu.v
										case 4:
											return 470; //vlseg5bu.v
										case 5:
											return 484; //vlseg6bu.v
										case 6:
											return 498; //vlseg7bu.v
										case 7:
											return 512; //vlseg8bu.v
									}break;
								case 16:
									switch(nf){
										case 0:
											return 423; //vlbuff.v
										case 1:
											return 429; //vlseg2buff.v
										case 2:
											return 443; //vlseg3buff.v
										case 3:
											return 457; //vlseg4buff.v
										case 4:
											return 471; //vlseg5buff.v
										case 5:
											return 485; //vlseg6buff.v
										case 6:
											return 499; //vlseg7buff.v
										case 7:
											return 513; //vlseg8buff.v
									}break;
							}; break;
						case 5:
							switch(rs2){
								case 0:
									switch(nf){
										case 0:
											return 401; //vlhu.v
										case 1:
											return 430; //vlseg2hu.v
										case 2:
											return 444; //vlseg3hu.v
										case 3:
											return 458; //vlseg4hu.v
										case 4:
											return 472; //vlseg5hu.v
										case 5:
											return 486; //vlseg6hu.v
										case 6:
											return 500; //vlseg7hu.v
										case 7:
											return 514; //vlseg8hu.v
									}break;
								case 16:
									switch(nf){
										case 0:
											return 425; //vlhuff.v
										case 1:
											return 431; //vlseg2huff.v
										case 2:
											return 445; //vlseg3huff.v
										case 3:
											return 459; //vlseg4huff.v
										case 4:
											return 473; //vlseg5huff.v
										case 5:
											return 487; //vlseg6huff.v
										case 6:
											return 501; //vlseg7huff.v
										case 7:
											return 515; //vlseg8huff.v
									}break;
							}; break;
						case 6:
							switch(rs2){
								case 0:
									switch(nf){
										case 0:
											return 408; //vlwu.v
										case 1:
											return 432; //vlseg2wu.v
										case 2:
											return 446; //vlseg3wu.v
										case 3:
											return 460; //vlseg4wu.v
										case 4:
											return 474; //vlseg5wu.v
										case 5:
											return 488; //vlseg6wu.v
										case 6:
											return 502; //vlseg7wu.v
										case 7:
											return 516; //vlseg8wu.v
									}break;
								case 16:
									switch(nf){
										case 0:
											return 427; //vlwuff.v
										case 1:
											return 433; //vlseg2wuff.v
										case 2:
											return 447; //vlseg3wuff.v
										case 3:
											return 461; //vlseg4wuff.v
										case 4:
											return 475; //vlseg5wuff.v
										case 5:
											return 489; //vlseg6wuff.v
										case 6:
											return 503; //vlseg7wuff.v
										case 7:
											return 517; //vlseg8wuff.v
									}break;
							}; break;
						case 7:
							switch(rs2){
								case 0:
									switch(nf){
										case 0:
											return 409; //vle.v
										case 1:
											return 434; //vlseg2e.v
										case 2:
											return 448; //vlseg3e.v
										case 3:
											return 462; //vlseg4e.v
										case 4:
											return 476; //vlseg5e.v
										case 5:
											return 490; //vlseg6e.v
										case 6:
											return 504; //vlseg7e.v
										case 7:
											return 518; //vlseg8e.v
									}break;
								case 16:
									switch(nf){
										case 0:
											return 421; //vleff.v
										case 1:
											return 435; //vlseg2eff.v
										case 2:
											return 449; //vlseg3eff.v
										case 3:
											return 463; //vlseg4eff.v
										case 4:
											return 477; //vlseg5eff.v
										case 5:
											return 491; //vlseg6eff.v
										case 6:
											return 505; //vlseg7eff.v
										case 7:
											return 519; //vlseg8eff.v
									}break;
							}; break;
					} break;
				case 2: 
					switch(funct3){
						case 0:
							switch(nf){
								case 0:
									return 410; //vlsbu.v
								case 1:
									return 526; //vlsseg2bu.v
								case 2:
									return 533; //vlsseg3bu.v
								case 3:
									return 540; //vlsseg4bu.v
								case 4:
									return 547; //vlsseg5bu.v
								case 5:
									return 554; //vlsseg6bu.v
								case 6:
									return 561; //vlsseg7bu.v
								case 7:
									return 568; //vlsseg8bu.v
							}break;
						case 5:
							switch(nf){
								case 0:
									return 412; //vlshu.v
								case 1:
									return 527; //vlsseg2hu.v
								case 2:
									return 534; //vlsseg3hu.v
								case 3:
									return 541; //vlsseg4hu.v
								case 4:
									return 548; //vlsseg5hu.v
								case 5:
									return 555; //vlsseg6hu.v
								case 6:
									return 562; //vlsseg7hu.v
								case 7:
									return 569; //vlsseg8hu.v
							}break;
						case 6:
							switch(nf){
								case 0:
									return 414; //vlswu.v
								case 1:
									return 528; //vlsseg2wu.v
								case 2:
									return 535; //vlsseg3wu.v
								case 3:
									return 542; //vlsseg4wu.v
								case 4:
									return 549; //vlsseg5wu.v
								case 5:
									return 556; //vlsseg6wu.v
								case 6:
									return 563; //vlsseg7wu.v
								case 7:
									return 570; //vlsseg8wu.v
							}break;
						case 7:
							switch(nf){
								case 0:
									return 406; //vlse.v
								case 1:
									return 529; //vlsseg2e.v
								case 2:
									return 536; //vlsseg3e.v
								case 3:
									return 543; //vlsseg4e.v
								case 4:
									return 550; //vlsseg5e.v
								case 5:
									return 557; //vlsseg6e.v
								case 6:
									return 564; //vlsseg7e.v
								case 7:
									return 571; //vlsseg8e.v
							}break;
					} break;
				case 3: 
					switch(funct3){
						case 0:
							switch(nf){
								case 0:
									return 416; //vlxbu.v
								case 1:
									return 575; //vlxseg2bu.v
								case 2:
									return 582; //vlxseg3bu.v
								case 3:
									return 589; //vlxseg4bu.v
								case 4:
									return 596; //vlxseg5bu.v
								case 5:
									return 603; //vlxseg6bu.v
								case 6:
									return 610; //vlxseg7bu.v
								case 7:
									return 617; //vlxseg8bu.v
							}break;
						case 5:
							switch(nf){
								case 0:
									return 418; //vlxhu.v
								case 1:
									return 576; //vlxseg2hu.v
								case 2:
									return 583; //vlxseg3hu.v
								case 3:
									return 590; //vlxseg4hu.v
								case 4:
									return 597; //vlxseg5hu.v
								case 5:
									return 604; //vlxseg6hu.v
								case 6:
									return 611; //vlxseg7hu.v
								case 7:
									return 618; //vlxseg8hu.v
							}break;
						case 6:
							switch(nf){
								case 0:
									return 420; //vlxwu.v
								case 1:
									return 577; //vlxseg2wu.v
								case 2:
									return 584; //vlxseg3wu.v
								case 3:
									return 591; //vlxseg4wu.v
								case 4:
									return 598; //vlxseg5wu.v
								case 5:
									return 605; //vlxseg6wu.v
								case 6:
									return 612; //vlxseg7wu.v
								case 7:
									return 619; //vlxseg8wu.v
							}break;
						case 7:
							switch(nf){
								case 0:
									return 400; //vlxe.v
								case 1:
									return 578; //vlxseg2e.v
								case 2:
									return 585; //vlxseg3e.v
								case 3:
									return 592; //vlxseg4e.v
								case 4:
									return 599; //vlxseg5e.v
								case 5:
									return 606; //vlxseg6e.v
								case 6:
									return 613; //vlxseg7e.v
								case 7:
									return 620; //vlxseg8e.v
							}break;
					} break;
				case 4: 
					switch(funct3){
						case 0:
							switch(rs2){
								case 0:
									switch(nf){
										case 0:
											return 403; //vlb.v
										case 1:
											return 436; //vlseg2b.v
										case 2:
											return 450; //vlseg3b.v
										case 3:
											return 464; //vlseg4b.v
										case 4:
											return 478; //vlseg5b.v
										case 5:
											return 492; //vlseg6b.v
										case 6:
											return 506; //vlseg7b.v
										case 7:
											return 520; //vlseg8b.v
									}break;
								case 16:
									switch(nf){
										case 0:
											return 422; //vlbff.v
										case 1:
											return 437; //vlseg2bff.v
										case 2:
											return 451; //vlseg3bff.v
										case 3:
											return 465; //vlseg4bff.v
										case 4:
											return 479; //vlseg5bff.v
										case 5:
											return 493; //vlseg6bff.v
										case 6:
											return 507; //vlseg7bff.v
										case 7:
											return 521; //vlseg8bff.v
									}break;
							}; break;
						case 5:
							switch(rs2){
								case 0:
									switch(nf){
										case 0:
											return 405; //vlh.v
										case 1:
											return 438; //vlseg2h.v
										case 2:
											return 452; //vlseg3h.v
										case 3:
											return 466; //vlseg4h.v
										case 4:
											return 480; //vlseg5h.v
										case 5:
											return 494; //vlseg6h.v
										case 6:
											return 508; //vlseg7h.v
										case 7:
											return 522; //vlseg8h.v
									}break;
								case 16:
									switch(nf){
										case 0:
											return 424; //vlhff.v
										case 1:
											return 439; //vlseg2hff.v
										case 2:
											return 453; //vlseg3hff.v
										case 3:
											return 467; //vlseg4hff.v
										case 4:
											return 481; //vlseg5hff.v
										case 5:
											return 495; //vlseg6hff.v
										case 6:
											return 509; //vlseg7hff.v
										case 7:
											return 523; //vlseg8hff.v
									}break;
							}; break;
						case 6:
							switch(rs2){
								case 0:
									switch(nf){
										case 0:
											return 407; //vlw.v
										case 1:
											return 440; //vlseg2w.v
										case 2:
											return 454; //vlseg3w.v
										case 3:
											return 468; //vlseg4w.v
										case 4:
											return 482; //vlseg5w.v
										case 5:
											return 496; //vlseg6w.v
										case 6:
											return 510; //vlseg7w.v
										case 7:
											return 524; //vlseg8w.v
									}break;
								case 16:
									switch(nf){
										case 0:
											return 426; //vlwff.v
										case 1:
											return 441; //vlseg2wff.v
										case 2:
											return 455; //vlseg3wff.v
										case 3:
											return 469; //vlseg4wff.v
										case 4:
											return 483; //vlseg5wff.v
										case 5:
											return 497; //vlseg6wff.v
										case 6:
											return 511; //vlseg7wff.v
										case 7:
											return 525; //vlseg8wff.v
									}break;
							}; break;
					} break;
				case 6: 
					switch(funct3){
						case 0:
							switch(nf){
								case 0:
									return 402; //vlsb.v
								case 1:
									return 530; //vlsseg2b.v
								case 2:
									return 537; //vlsseg3b.v
								case 3:
									return 544; //vlsseg4b.v
								case 4:
									return 551; //vlsseg5b.v
								case 5:
									return 558; //vlsseg6b.v
								case 6:
									return 565; //vlsseg7b.v
								case 7:
									return 572; //vlsseg8b.v
							}break;
						case 5:
							switch(nf){
								case 0:
									return 411; //vlsh.v
								case 1:
									return 531; //vlsseg2h.v
								case 2:
									return 538; //vlsseg3h.v
								case 3:
									return 545; //vlsseg4h.v
								case 4:
									return 552; //vlsseg5h.v
								case 5:
									return 559; //vlsseg6h.v
								case 6:
									return 566; //vlsseg7h.v
								case 7:
									return 573; //vlsseg8h.v
							}break;
						case 6:
							switch(nf){
								case 0:
									return 413; //vlsw.v
								case 1:
									return 532; //vlsseg2w.v
								case 2:
									return 539; //vlsseg3w.v
								case 3:
									return 546; //vlsseg4w.v
								case 4:
									return 553; //vlsseg5w.v
								case 5:
									return 560; //vlsseg6w.v
								case 6:
									return 567; //vlsseg7w.v
								case 7:
									return 574; //vlsseg8w.v
							}break;
					} break;
				case 7: 
					switch(funct3){
						case 0:
							switch(nf){
								case 0:
									return 415; //vlxb.v
								case 1:
									return 579; //vlxseg2b.v
								case 2:
									return 586; //vlxseg3b.v
								case 3:
									return 593; //vlxseg4b.v
								case 4:
									return 600; //vlxseg5b.v
								case 5:
									return 607; //vlxseg6b.v
								case 6:
									return 614; //vlxseg7b.v
								case 7:
									return 621; //vlxseg8b.v
							}break;
						case 5:
							switch(nf){
								case 0:
									return 417; //vlxh.v
								case 1:
									return 580; //vlxseg2h.v
								case 2:
									return 587; //vlxseg3h.v
								case 3:
									return 594; //vlxseg4h.v
								case 4:
									return 601; //vlxseg5h.v
								case 5:
									return 608; //vlxseg6h.v
								case 6:
									return 615; //vlxseg7h.v
								case 7:
									return 622; //vlxseg8h.v
							}break;
						case 6:
							switch(nf){
								case 0:
									return 419; //vlxw.v
								case 1:
									return 581; //vlxseg2w.v
								case 2:
									return 588; //vlxseg3w.v
								case 3:
									return 595; //vlxseg4w.v
								case 4:
									return 602; //vlxseg5w.v
								case 5:
									return 609; //vlxseg6w.v
								case 6:
									return 616; //vlxseg7w.v
								case 7:
									return 623; //vlxseg8w.v
							}break;
					} break;
			}
			break;
		case STOREFP:
			nf=(instr>>29)&0x7;
			switch(mop){
				case 0: 
					switch(funct3){
						case 0:
							switch(nf){
								case 0:
									return 802; //vsb.v
								case 1:
									return 816; //vsseg2b.v
								case 2:
									return 820; //vsseg3b.v
								case 3:
									return 824; //vsseg4b.v
								case 4:
									return 828; //vsseg5b.v
								case 5:
									return 832; //vsseg6b.v
								case 6:
									return 836; //vsseg7b.v
								case 7:
									return 840; //vsseg8b.v
							}break;
						case 5:
							switch(nf){
								case 0:
									return 805; //vsh.v
								case 1:
									return 817; //vsseg2h.v
								case 2:
									return 821; //vsseg3h.v
								case 3:
									return 825; //vsseg4h.v
								case 4:
									return 829; //vsseg5h.v
								case 5:
									return 833; //vsseg6h.v
								case 6:
									return 837; //vsseg7h.v
								case 7:
									return 841; //vsseg8h.v
							}break;
						case 6:
							switch(nf){
								case 0:
									return 806; //vsw.v
								case 1:
									return 818; //vsseg2w.v
								case 2:
									return 822; //vsseg3w.v
								case 3:
									return 826; //vsseg4w.v
								case 4:
									return 830; //vsseg5w.v
								case 5:
									return 834; //vsseg6w.v
								case 6:
									return 838; //vsseg7w.v
								case 7:
									return 842; //vsseg8w.v
							}break;
						case 7:
							return 813; //vse.v
							switch(nf){
								case 0:
									return 813; //vse.v
								case 1:
									return 819; //vsseg2e.v
								case 2:
									return 823; //vsseg3e.v
								case 3:
									return 827; //vsseg4e.v
								case 4:
									return 831; //vsseg5e.v
								case 5:
									return 835; //vsseg6e.v
								case 6:
									return 839; //vsseg7e.v
								case 7:
									return 843; //vsseg8e.v
							}break;
					} break;

				case 2: 
					switch(funct3){
						case 0:
							switch(nf){
								case 0:
									return 807; //vssb.v
								case 1:
									return 844; //vssseg2b.v
								case 2:
									return 848; //vssseg3b.v
								case 3:
									return 852; //vssseg4b.v
								case 4:
									return 856; //vssseg5b.v
								case 5:
									return 860; //vssseg6b.v
								case 6:
									return 864; //vssseg7b.v
								case 7:
									return 868; //vssseg8b.v
							}break;
						case 5:
							switch(nf){
								case 0:
									return 803; //vssh.v
								case 1:
									return 845; //vssseg2h.v
								case 2:
									return 849; //vssseg3h.v
								case 3:
									return 853; //vssseg4h.v
								case 4:
									return 857; //vssseg5h.v
								case 5:
									return 861; //vssseg6h.v
								case 6:
									return 865; //vssseg7h.v
								case 7:
									return 869; //vssseg8h.v
							}break;
						case 6:
							switch(nf){
								case 0:
									return 809; //vssw.v
								case 1:
									return 846; //vssseg2w.v
								case 2:
									return 850; //vssseg3w.v
								case 3:
									return 854; //vssseg4w.v
								case 4:
									return 858; //vssseg5w.v
								case 5:
									return 862; //vssseg6w.v
								case 6:
									return 866; //vssseg7w.v
								case 7:
									return 870; //vssseg8w.v
							}break;
						case 7:
							switch(nf){
								case 0:
									return 804; //vsse.v
								case 1:
									return 847; //vssseg2e.v
								case 2:
									return 851; //vssseg3e.v
								case 3:
									return 855; //vssseg4e.v
								case 4:
									return 859; //vssseg5e.v
								case 5:
									return 863; //vssseg6e.v
								case 6:
									return 867; //vssseg7e.v
								case 7:
									return 871; //vssseg8e.v
							}break;
					} break;
				case 3: 
					switch(funct3){
						case 0:
							switch(nf){
								case 0:
									return 810; //vsxb.v
								case 1:
									return 872; //vsxseg2b.v
								case 2:
									return 876; //vsxseg3b.v
								case 3:
									return 880; //vsxseg4b.v
								case 4:
									return 884; //vsxseg5b.v
								case 5:
									return 888; //vsxseg6b.v
								case 6:
									return 892; //vsxseg7b.v
								case 7:
									return 896; //vsxseg8b.v
							}break;
						case 5:
							switch(nf){
								case 0:
									return 811; //vsxh.v
								case 1:
									return 873; //vsxseg2h.v
								case 2:
									return 877; //vsxseg3h.v
								case 3:
									return 881; //vsxseg4h.v
								case 4:
									return 885; //vsxseg5h.v
								case 5:
									return 889; //vsxseg6h.v
								case 6:
									return 893; //vsxseg7h.v
								case 7:
									return 897; //vsxseg8h.v
							}break;
						case 6:
							switch(nf){
								case 0:
									return 812; //vsxw.v
								case 1:
									return 874; //vsxseg2w.v
								case 2:
									return 878; //vsxseg3w.v
								case 3:
									return 882; //vsxseg4w.v
								case 4:
									return 886; //vsxseg5w.v
								case 5:
									return 890; //vsxseg6w.v
								case 6:
									return 894; //vsxseg7w.v
								case 7:
									return 898; //vsxseg8w.v
							}break;
						case 7:
							switch(nf){
								case 0:
									return 808; //vsxe.v
								case 1:
									return 875; //vsxseg2e.v
								case 2:
									return 879; //vsxseg3e.v
								case 3:
									return 883; //vsxseg4e.v
								case 4:
									return 887; //vsxseg5e.v
								case 5:
									return 891; //vsxseg6e.v
								case 6:
									return 895; //vsxseg7e.v
								case 7:
									return 899; //vsxseg8e.v
							}break;
					} break;
				case 7: 
					switch(funct3){
						case 0:
							switch(nf){
								case 0:
									return 801; //vsuxb.v
								case 1:
									return 900; //vsuxseg2b.v
								case 2:
									return 904; //vsuxseg3b.v
								case 3:
									return 908; //vsuxseg4b.v
								case 4:
									return 912; //vsuxseg5b.v
								case 5:
									return 916; //vsuxseg6b.v
								case 6:
									return 920; //vsuxseg7b.v
								case 7:
									return 924; //vsuxseg8b.v
							}break;
						case 5:
							switch(nf){
								case 0:
									return 814; //vsuxh.v
								case 1:
									return 901; //vsuxseg2h.v
								case 2:
									return 905; //vsuxseg3h.v
								case 3:
									return 909; //vsuxseg4h.v
								case 4:
									return 913; //vsuxseg5h.v
								case 5:
									return 917; //vsuxseg6h.v
								case 6:
									return 921; //vsuxseg7h.v
								case 7:
									return 925; //vsuxseg8h.v
							}break;
						case 6:
							switch(nf){
								case 0:
									return 815; //vsuxw.v
								case 1:
									return 902; //vsuxseg2w.v
								case 2:
									return 906; //vsuxseg3w.v
								case 3:
									return 910; //vsuxseg4w.v
								case 4:
									return 914; //vsuxseg5w.v
								case 5:
									return 918; //vsuxseg6w.v
								case 6:
									return 922; //vsuxseg7w.v
								case 7:
									return 926; //vsuxseg8w.v
							}break;
						case 7:
							switch(nf){
								case 0:
									return 800; //vsuxe.v
								case 1:
									return 903; //vsuxseg2e.v
								case 2:
									return 907; //vsuxseg3e.v
								case 3:
									return 911; //vsuxseg4e.v
								case 4:
									return 915; //vsuxseg5e.v
								case 5:
									return 919; //vsuxseg6e.v
								case 6:
									return 923; //vsuxseg7e.v
								case 7:
									return 927; //vsuxseg8e.v
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
				unsigned int sew = 1 << (((instr>>22)&0x7)+3);
				unsigned int lmul = (((instr>>20)&0x3)+1);
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
							return 106; //vfredsum.vs
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
							return 3; //vrsub.vx
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
							return 110; //vfsgnjn.vv
						case 5: //OPFVF
							return 110; //vfsgnjn.vf
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
							return 119; //vfsgnjx.vv
						case 5: //OPFVF
							return 119; //vfsgnjx.vf
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
					} break;
				case 12:
					switch(funct3){
						case 0: //OPIVV
							return 203; //vrgather.vv
						case 4: //OPIVX
							return 203; //vrgather.vx
						case 3: //OPIVI
							return 203; //vrgather.vi
						case 2: //OPMVV
							return 201; //vmv.x.s
							return 230; //vext.x.v
							break;
						case 1: //OPFVV
							return 202; //vfmv.f.s
					} break;
				case 13:
					switch(funct3){
						case 6: //OPMVX
							return 201; //vmv.s.x
						case 5: //OPFVF
							return 202; //vfmv.s.f
					} break;
				case 14:
					switch(funct3){
						case 4: //OPIVX
							return 222; //vslideup.vx
						case 3: //OPIVI
							return 222; //vslideup.vi
						case 6: //OPMVX
							return 223; //vslide1up.vx
					} break;
				case 15:
					switch(funct3){
						case 4: //OPIVX
							return 220; //vslidedown.vx
						case 3: //OPIVI
							return 220; //vslidedown.vi
						case 6: //OPMVX
							return 221; //vslide1down.vx
					} break;
				case 16:
					switch(funct3){
						case 0: //OPIVV
							return 40; //vadc.vvm
						case 4: //OPIVX
							return 40; //vadc.vxm
						case 3: //OPIVI
							return 40; //vadc.vim
					} break;
				case 17:
					switch(funct3){
						case 0: //OPIVV
							return 41; //vmadc.vvm
						case 4: //OPIVX
							return 41; //vmadc.vxm
						case 3: //OPIVI
							return 41; //vmadc.vim
					} break;
				case 18:
					switch(funct3){
						case 0: //OPIVV
							return 42; //vsbc.vvm
						case 4: //OPIVX
							return 42; //vsbc.vxm
					} break;
				case 19:
					switch(funct3){
						case 0: //OPIVV
							return 43; //vmsbc.vvm
						case 4: //OPIVX
							return 43; //vmsbc.vxm
					} break;
				case 20:
					switch(funct3){
						case 2: //OPMVV
							return 231; //vmpopc.m
					} break;
				case 21:
					switch(funct3){
						case 2: //OPMVV
							return 232; //vmfirst.m
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
							return 318; //vmandnot.mm
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
							return 326; //vmcpy.m
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
						case 1: //OPFVV
							return 328; //vmford.vv
						case 5: //OPFVF
							return 328; //vmford.vf
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
							return 321; //vmornot.mm
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
								case 16:
									return 244; //vfncvt.xu.f.v
								case 17:
									return 244; //vfncvt.x.f.v
								case 18:
									return 244; //vfncvt.f.xu.v
								case 19:
									return 244; //vfncvt.f.x.v
								case 20:
									return 244; //vfncvt.f.f.v
							}break;
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
						case 1: //OPFVV
							switch(rs1){
								case 0:
									return 109; //vfsqrt.v
								case 16:
									return 247; //vfclass.v
							}break;
					} break;
				case 36:
					switch(funct3){
						case 0: //OPIVV
							return 51; //vaadd.vv
						case 4: //OPIVX
							return 51; //vaadd.vx
						case 3: //OPIVI
							return 51; //vaadd.vi
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
						case 0: //OPIVV
							return 52; //vasub.vv
						case 4: //OPIVX
							return 52; //vasub.vx
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
							return 55; //vnsrl.vv
						case 4: //OPIVX
							return 55; //vnsrl.vx
						case 3: //OPIVI
							return 55; //vnsrl.vi
						case 1: //OPFVV
							return 108; //vfmacc.vv
						case 5: //OPFVF
							return 108; //vfmacc.vf
					} break;
				case 45:
					switch(funct3){
						case 0: //OPIVV
							return 56; //vnsra.vv
						case 4: //OPIVX
							return 56; //vnsra.vx
						case 3: //OPIVI
							return 56; //vnsra.vi
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
							return 246; //vnclipu.vv
						case 4: //OPIVX
							return 246; //vnclipu.vx
						case 3: //OPIVI
							return 246; //vnclipu.vi
						case 1: //OPFVV
							return 111; //vfmsac.vv
						case 5: //OPFVF
							return 111; //vfmsac.vf
					} break;
				case 47:
					switch(funct3){
						case 0: //OPIVV
							return 245; //vnclip.vv
						case 4: //OPIVX
							return 245; //vnclip.vx
						case 3: //OPIVI
							return 245; //vnclip.vi
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
							return 126; //vfwredsum.vs
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
						case 0: //OPIVV
							return 23; //vdotu.vv
						case 2: //OPMVV
							return 63; //vwmulu.vv
						case 6: //OPMVX
							return 63; //vwmulu.vx
						case 1: //OPFVV
							return 129; //vfwmul.vv
						case 5: //OPFVF
							return 129; //vfwmul.vf
					} break;
				case 57:
					switch(funct3){
						case 0: //OPIVV
							return 22; //vdot.vv
						case 1: //OPFVV
							return 130; //vfdot.vv
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
						case 0: //OPIVV
							return 72; //vwsmaccu.vv
						case 4: //OPIVX
							return 72; //vwsmaccu.vx
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
						case 0: //OPIVV
							return 73; //vwsmacc.vv
						case 4: //OPIVX
							return 73; //vwsmacc.vx
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
						case 0: //OPIVV
							return 70; //vwsmaccsu.vv
						case 4: //OPIVX
							return 70; //vwsmaccsu.vx
						case 2: //OPMVV
							return 68; //vwmaccsu.vv
						case 6: //OPMVX
							return 68; //vwmaccsu.vx
						case 1: //OPFVV
							return 133; //vfwmsac.vv
						case 5: //OPFVF
							return 133; //vfwmsac.vf
					} break;
				case 63:
					switch(funct3){
						case 4: //OPIVX
							return 71; //vwsmaccus.vx
						case 6: //OPMVX
							return 69; //vwmaccus.vx
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
