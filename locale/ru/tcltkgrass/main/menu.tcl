frame .main_menu
pack .main_menu -expand yes -fill both

# menus used several times in the main menu

set monitors_menu {
    "�����" "" {
        "��� �������� X" "" {start_monitors}
        -separator
        X0 "" {"start_monitor x0"}
        X1 "" {"start_monitor x1"}
        X2 "" {"start_monitor x2"}
        X3 "" {"start_monitor x3"}
        X4 "" {"start_monitor x4"}
        X5 "" {"start_monitor x5"}
        X6 "" {"start_monitor x6"}
        -separator
        CELL "" {"exec xterm -iconic -e d.mon start=CELL"}
    }
    "����" "" {
        "��� X" "" {stop_monitors}
        -separator
        X0 "" {"stop_monitor x0"}
        X1 "" {"stop_monitor x1"}
        X2 "" {"stop_monitor x2"}
        X3 "" {"stop_monitor x3"}
        X4 "" {"stop_monitor x4"}
        X5 "" {"stop_monitor x5"}
        X6 "" {"stop_monitor x6"}
        -separator
        CELL "" {"exec xterm -iconic -e d.mon stop=CELL"}
    }
    "�������" "" {
        X0 "" {"exec xterm -iconic -e d.mon select=x0"}
        X1 "" {"exec xterm -iconic -e d.mon select=x1"}
        X2 "" {"exec xterm -iconic -e d.mon select=x2"}
        X3 "" {"exec xterm -iconic -e d.mon select=x3"}
        X4 "" {"exec xterm -iconic -e d.mon select=x4"}
        X5 "" {"exec xterm -iconic -e d.mon select=x5"}
        X6 "" {"exec xterm -iconic -e d.mon select=x6"}
        -separator
        CELL "" {"exec xterm -iconic -e d.mon select=CELL"}
    }
    "��������� X-�������" "" {
        "source $env(TCLTKGRASSBASE)/module/d.mon"
    }
}

set display_raster {
    "�������� ��������� �����" "" {
        "source $env(TCLTKGRASSBASE)/module/d.rast"
    }
    "�������� �������� HIS" "" {
        "source $env(TCLTKGRASSBASE)/module/d.his"
    }
    "��������� RGB" "" {
        "source $env(TCLTKGRASSBASE)/module/d.rgb"
    }
    "3D-�����������" "" {
        "source $env(TCLTKGRASSBASE)/module/d.3d"
    }
    "������� �����" "" {
        "source $env(TCLTKGRASSBASE)/module/d.shadedmap"
    }
    "�������" "" {
        "source $env(TCLTKGRASSBASE)/module/d.profile"
    }
}

set display_vector {
    "�������� ��������� �����" "" {
        "source $env(TCLTKGRASSBASE)/module/d.vect"
    }
    "�������� ������� � �������" "" {
        "source $env(TCLTKGRASSBASE)/module/d.vect.area"
    }
    "�������� ����� � �������" "" {
    	"source $env(TCLTKGRASSBASE)/module/d.vect.line"
    }
}
set display_sites {
    "�������� �������" "" {
        "source $env(TCLTKGRASSBASE)/module/d.sites"
    }
    "�������� �������� ��������" "" {
        "source $env(TCLTKGRASSBASE)/module/d.site.labels"
    }
    -separator
    "�������� �����" "" {
        "source $env(TCLTKGRASSBASE)/module/d.points"
    }
    "�������� ����� (����.)" "" {
        "source $env(TCLTKGRASSBASE)/module/d.icons"
    }
}

set image_processing {
    "�������/������������� ������" "" {
        "run i.group &"
    }
    "����� ���� ��� ������" "" {
        "run i.target &"
    }
    -separator
    ������������ "" {
         "����� ����������� ����� (GCP)" "" {
             "run i.points &"
         }
         "������� � ���������� ��������������" "" {
             "run i.rectify &"
         }
         
         "����-����������������" "" {
             "run i.ortho.photo &"
         }
    }
    "�������" "" {
         "���������� ������ ZECD" "" {
             "source $env(TCLTKGRASSBASE)/module/i.zc"
         }
         "���������� �������� ��������" "" {
             "source $env(TCLTKGRASSBASE)/module/r.mfilter"
         }
	 "������� ������ ����� �� �����������" "" {
	     "source $env(TCLTKGRASSBASE)/module/i.grey.scale"
	 }
    }
    "��������������" "" {
         "������������ ����������" "" {
             "source $env(TCLTKGRASSBASE)/module/i.cca"
         }
         "������� ����������" "" {
             "source $env(TCLTKGRASSBASE)/module/i.pca"
         }
         "������� �������������� ����� (FFT)" "" {
             "source $env(TCLTKGRASSBASE)/module/i.fft"
         }
         "������/������� ����� ��� FFT" "" {
             "run r.mask &"
         }
         "�������� ������� �������������� �����" "" {
             "source $env(TCLTKGRASSBASE)/module/i.ifft"
         }
    }
    ������������� "" {
         "���������� ���� ��� �������������� �������������" "" {
             "source $env(TCLTKGRASSBASE)/module/i.cluster"
         }
         "������������� ���� ��� ���������������� �������������" "" {
             "run i.class &"
         }
         "��������������� ���� ��� ���������������� �������������(MLC)" "" {
             "source $env(TCLTKGRASSBASE)/module/i.gensig"
         }
         "��������������� ���� ��� ���������������� �������������(SMAP)" "" {
             "source $env(TCLTKGRASSBASE)/module/i.gensigset"
         }
         -separator
         "������������� ������������� �������������(MLC)" "" {
             "source $env(TCLTKGRASSBASE)/module/i.maxlik"
         }
         "������������� ����������������� ��������� (SMAP)" "" {
             "source $env(TCLTKGRASSBASE)/module/i.smap"
         }
    }
}

set misc {
    "�������������� ���������" "" {
	"��������/����������" "" {
	    "run m.proj &"
	}
	"��������� ��������" "" {
	    "source $env(TCLTKGRASSBASE)/module/m.datum.shift"
	}
	"��������.- ���/���" "" {
	    "source $env(TCLTKGRASSBASE)/module/m.gc2ll"
	}
	"���/��� - ��������." "" {
	    "source $env(TCLTKGRASSBASE)/module/m.ll2gc"
	}
	"UTM - ���/���" "" {
	    "source $env(TCLTKGRASSBASE)/module/m.u2ll"
	}
	"���/��� - UTM" "" {
	    "source $env(TCLTKGRASSBASE)/module/m.ll2u"
	}
    }
    -separator
    "DEM/DTED" "" {
	"����������� DEM" "" {
	    "source $env(TCLTKGRASSBASE)/module/m.dem.examine"
	}
	"��������� DEM" "" {
	    "source $env(TCLTKGRASSBASE)/module/m.dem.extract"
	}
	"����������� DTED" "" {
	    "source $env(TCLTKGRASSBASE)/module/m.dted.examine"
	}
	"��������� DTED" "" {
	    "source $env(TCLTKGRASSBASE)/module/m.dted.extract"
	}
    }
    -separator
    "������" "" {
	"������� ������ ����� �� 90 ����." "" {
	    "source $env(TCLTKGRASSBASE)/module/m.rot90"
	}
	"���������� ����������� �����" "" {
	    "source $env(TCLTKGRASSBASE)/module/m.flip"
	}
	"CTG data  �� USGS lulc-�����" "" {
	    "source $env(TCLTKGRASSBASE)/module/m.lulc.USGS"
	}
	"���������� �� Tiger Region" "" {
	    "source $env(TCLTKGRASSBASE)/module/m.tiger.region"
	}
	"UTM ������ - lat/lon ������" "" {
	    "source $env(TCLTKGRASSBASE)/module/m.region.ll"
	}
    }
}

# main menu

menu_build 1 .main_menu {
    ����� "��������� TCLTKGRASS" {
        ������� "" $monitors_menu
        "���� �������" "" {
            "������ ������ �������������" "" {
                "resize_menu; resize $module_list"
            }
            "������ ������� ��� �������� ����" "" {
                "resize_menu; reinit_modules"
            }
            -separator
            "������� ��� ���� �������" "" {
                unmap_modules
            }
            "������������ ��� �������� ����" "" {
                map_modules
            }
            -separator
            "��������� ��� �������� ����" "" {
                "foreach module $module_list {catch {wm iconify .$module; \
                                                     wm iconify .$module.print}}"
            }
            "�������� ��� �������� ����" "" {
                "foreach module $module_list {catch {wm deiconify .$module; \
                                                     wm deiconify .$module.print}}"
            }
            -separator
            "������� ��� �������� ����" "" {
                "foreach module $module_list {catch {destroy .$module}}"
            }
        }
        "������ ���� �������������" "" {resize_menu}
        -separator
	������� "" {
	    "������������� ������" "" {
		"script_start"
	    }
	    "���������� ������"  "" {
		"script_stop"
	    }
	    "������ �������"     "" {
		"script_play"
	    }
	}

	-separator
        ����� "" {
	"���� ����" "" {
                "fontsel {Menu font} main_menu(font);\
                 setfont .main_menu $main_menu(font);\
                 resize_menu"
            }
            "���� �������" "" {
                "fontsel {Module font} module_font;\
                 foreach module $module_list {setfont .$module $module_font} ;\
                 resize $module_list"
            }
            "���� ����������" "" {
                "fontsel {Result font} result_font"
            }
            "���� �������" "" {
                "fontsel {Dialog font} dialog_font"
            }
            -separator
            "������ �������" "" {
                setdisplay
            }
	    -separator
	    "������������ ��������" "" {
		"config_netscape"
	    }
        }
        -separator
        "��������� ���������" "" {
            "tcltkgrass_save ."
        }
    }
    ����� "��������� � �������� � �������" {
        "������" "" {
            "source $env(TCLTKGRASSBASE)/module/g.list"
        }
        "����������" "" {
            "source $env(TCLTKGRASSBASE)/module/g.copy"
        }
        "�������������" "" {
            "source $env(TCLTKGRASSBASE)/module/g.rename"
        }
        "�������" "" {
            "source $env(TCLTKGRASSBASE)/module/g.remove"
        }
        -separator
        "������ ������" "" {
            "run g.access &"
        }
        "�������� ����� ������" "" {
            "source $env(TCLTKGRASSBASE)/module/g.mapsets"
        }
        "������� ����� ������" "" {
            "source $env(TCLTKGRASSBASE)/module/mapset.remove"
        }
        -separator
        "�������/������������� ������" "" {
            "run i.group &"
        }
        "����� ���� ��� ������" "" {
            "run i.target &"
        }
    }
    ������ "������� �������" {
    	
        "���������/��������� �� ������" "" {
            "source $env(TCLTKGRASSBASE)/module/d.zoom"
        }
        "����������� �� ������" "" {
            "source $env(TCLTKGRASSBASE)/module/d.pan"
        }
        "����������/������� �����" "" {
            "run r.mask &"
        }
        -separator
	"������� ����� �� ������" "" {
            "source $env(TCLTKGRASSBASE)/main/monscale.tcl"
        }
        "�������� ��������� �������" "" {
            "run g.region -p &"
        }
        "������ �� ���������" "" {
            "exec g.region -d; exec d.erase"
        }
        "�������� ���������" "" {
            "source $env(TCLTKGRASSBASE)/module/g.region.sh"
        }
    }
    ������� "�������� �����" {
       ������� "" $monitors_menu
       "������� ����� �� ������" "" {
            "source $env(TCLTKGRASSBASE)/main/monscale.tcl"
        }
        -separator
        ����� "" $display_raster
        ������� "" $display_vector
        ������� "" $display_sites
        ����� "" {
            "�������� �������� �����" "" {
                "source $env(TCLTKGRASSBASE)/module/d.title.sh"
            }
            "�������� �������" "" {
                "source $env(TCLTKGRASSBASE)/module/d.legend"
            }
            "�������� ��������� �����" "" {
                "source $env(TCLTKGRASSBASE)/module/d.label"
            }
            "�������� ��������� ����� ��� paint" "" {
                "source $env(TCLTKGRASSBASE)/module/d.paint.labels"
            }
            "������� ����" "" {
                "source $env(TCLTKGRASSBASE)/module/d.font"
            }
            "�������� �����" "" {
                "source $env(TCLTKGRASSBASE)/module/d.text"
            }
        }
        ������� "" {
            "�������� �������� �������" "" {
                "source $env(TCLTKGRASSBASE)/module/d.colortable"
            }
            "�������� ��������" "" {
                "source $env(TCLTKGRASSBASE)/module/d.geodesic"
            }
            "�������� �����" "" {
                "source $env(TCLTKGRASSBASE)/module/d.rhumbline"
            }
            "���������� �������" "" {
                "source $env(TCLTKGRASSBASE)/module/d.barscale"
            }
            "�������� �����" "" {
                "source $env(TCLTKGRASSBASE)/module/d.grid"
            }
            "�������� �����������" "" {
                "source $env(TCLTKGRASSBASE)/module/d.histogram"
            }
            "�������� �������" "" {
                "source $env(TCLTKGRASSBASE)/module/d.legend"
            }
        }
        "���������������" "" {
            "source $env(TCLTKGRASSBASE)/module/d.where"
        }
	-separator
        "3D ������������ NVIZ" "" {
            "source $env(TCLTKGRASSBASE)/module/nviz"
        }
	"�������� �������" "" {
            "source $env(TCLTKGRASSBASE)/module/d.erase"
        }
        "���������/��������� �� ������" "" {
        "source $env(TCLTKGRASSBASE)/module/d.zoom"
        }
        "��������� ����� �������" "" {
            "source $env(TCLTKGRASSBASE)/module/d.frame"
        }
        "��� �������� �������" "" {
            "source $env(TCLTKGRASSBASE)/module/d.colormode"
        }
    }
    ����� "��������� ������" {
        �������� "" $display_raster
        "������" "" {
            "������ �����" "" {
                "source $env(TCLTKGRASSBASE)/module/d.what.rast"
            }
            "������ ����� ����" "" {
                "source $env(TCLTKGRASSBASE)/module/r.what"
            }
            "�������" "" {
                "source $env(TCLTKGRASSBASE)/module/d.profile"
            }
	    "�������� ����� � �������" "" {
                "source $env(TCLTKGRASSBASE)/module/d.measure"
            }
            "�������" "" {
                "������ �����" "" {
                    "run r.mask &"
                }
                "�����" "" {
                    "source $env(TCLTKGRASSBASE)/module/r.cross"
                }
                "��������" "" {
                    "source $env(TCLTKGRASSBASE)/module/r.patch"
                }
                "������� � ������� �� ��������" "" {
                    "source $env(TCLTKGRASSBASE)/module/r.infer"
                }
                "���������� ������� ������" "" {
                    "source $env(TCLTKGRASSBASE)/module/r.binfer"
                }
                "��������� ����������" "" {
                    "source $env(TCLTKGRASSBASE)/module/mapcalculator"
                }
                "���������� ��������" "" {
                    "source $env(TCLTKGRASSBASE)/module/r.combine"
                }
                "���������� �����" "" {
                    "run r.weight &"
                }
            }
            "�������������� �����������" "" {
                "������ �� �������� ������" "" {
                    "source $env(TCLTKGRASSBASE)/module/r.neighbors"
                }
                "�������" "" {
                    "source $env(TCLTKGRASSBASE)/module/r.buffer"
                }
                "���������� ������" "" {
                    "source $env(TCLTKGRASSBASE)/module/r.grow"
                }
                "���������� ��������� �����" "" {
                    "source $env(TCLTKGRASSBASE)/module/r.thin"
                }
            }
            "����������" "" {
                "�������� ���� ������������" "" {
                    "source $env(TCLTKGRASSBASE)/module/r.basins.fill"
                }
                "������������� ������������" "" {
                    "source $env(TCLTKGRASSBASE)/module/r.watershed"
                }
                "��������� ���� ����� ����� �������" "" {
                    "source $env(TCLTKGRASSBASE)/module/r.cost"
                }
                "���������� ���������� ������" "" {
                    "source $env(TCLTKGRASSBASE)/module/r.drain"
                }
                "���������� ����� ����� ����" "" {
                    "source $env(TCLTKGRASSBASE)/module/r.flow"
                }
                "����� � ����������" "" {
                    "source $env(TCLTKGRASSBASE)/module/r.slope.aspect"
                }
                "���� ������" "" {
                    "source $env(TCLTKGRASSBASE)/module/r.los"
                }
                "�������� �������� ������" "" {
                    "source $env(TCLTKGRASSBASE)/module/d.shadedmap"
                }
            }
        }
	"������������� �����" "" {
            "� �������" "" {
                "source $env(TCLTKGRASSBASE)/module/r.to.sites"
            }
        }
        "�������������" "" {
            "� ����� �� ������� ������" "" {
                "source $env(TCLTKGRASSBASE)/module/r.line"
            }
            "� ��������" "" {
                "source $env(TCLTKGRASSBASE)/module/r.poly"
            }
            "� ��������� �����" "" {
                "source $env(TCLTKGRASSBASE)/module/r.contour"
            }
        }
        "��������" "" {
            "�������/������������� ���� ��������" "" {
                "run r.support &"
            }
            "��������������� ���������" "" {
                "source $env(TCLTKGRASSBASE)/module/r.reclass"
            }
            "�������� ������� ���������" "" {
                "source $env(TCLTKGRASSBASE)/module/r.rescale"
            }
            "����������" "" {
                "source $env(TCLTKGRASSBASE)/module/r.average"
            }
            "�������������� ����������" "" {
                "source $env(TCLTKGRASSBASE)/module/r.statistics"
            }
            "��������� ����� �������" "" {
                "source $env(TCLTKGRASSBASE)/module/r.clump"
            }
            -separator
            "������� �������� �������" "" {
                "source $env(TCLTKGRASSBASE)/module/r.colors"
            }
            "������������� �������� �������" "" {
                "source $env(TCLTKGRASSBASE)/module/d.colors"
            }
	    "��. ������� �� ��������. ����������� �������� ������" "" {
		"source $env(TCLTKGRASSBASE)/module/i.grey.scale"
	    }
            -separator
            "����������" "" {
                "run r.digit &"
            }
            "��������� ����������" "" {
                "source $env(TCLTKGRASSBASE)/module/mapcalculator"
            }
            "������� ��������� �����" "" {
                "source $env(TCLTKGRASSBASE)/module/r.random"
            }
            -separator
            "�������� ��������� ������" "" {
                "source $env(TCLTKGRASSBASE)/module/r.resample"
            }
            "IDW-������������ �� ������ (���./����.)" "" {
                "source $env(TCLTKGRASSBASE)/module/r.surf.idw"
            }
            "IDW-������������ �� ������ (������ �����.)" "" {
                "source $env(TCLTKGRASSBASE)/module/r.surf.idw2"
            }
            "������������ �� �������������� ��������" "" {
                "source $env(TCLTKGRASSBASE)/module/r.surf.contour"
            }
            "������������ ��������� ���� �� ��������� �����" "" {
                "source $env(TCLTKGRASSBASE)/module/v.surf.rst"
            }
            -separator
            "�����/������� ������ �����" "" {
                "source $env(TCLTKGRASSBASE)/module/r.compress"
            }
        }
        "������" "" {
            "������� ����������" "" {
                "source $env(TCLTKGRASSBASE)/module/r.info"
            }
            -separator
            "����� ����������" "" {
                "source $env(TCLTKGRASSBASE)/module/r.stats"
            }
            "�������� � �������� ���������" "" {
                "source $env(TCLTKGRASSBASE)/module/r.cats"
            }
            "�������� ���������" "" {
                "source $env(TCLTKGRASSBASE)/module/r.describe"
            }
            -separator
            "�������" "" {
                "source $env(TCLTKGRASSBASE)/module/r.report"
            }
            "�����" "" {
                "source $env(TCLTKGRASSBASE)/module/r.volume"
            }
            "�������� ��������� ��������� (����������)" "" {
                "source $env(TCLTKGRASSBASE)/module/r.coin"
            }
            "�������� �� ����������" "" {
                "source $env(TCLTKGRASSBASE)/module/r.profile"
            }
            "�������� �� ���������� (���. ������ � ����������)" "" {
                "source $env(TCLTKGRASSBASE)/module/r.transect"
            }
            "������� ����������" "" {
                "source $env(TCLTKGRASSBASE)/module/r.covar"
            }
        }
        -separator
        "��������� �������" "" $image_processing
    }
    ������� "������ ��������� ����" {
        �������� "" $display_vector
        "������" "" {
                "������ �����" "" {
                       "source $env(TCLTKGRASSBASE)/module/d.what.vect"
                }
                "������ ����� ����" "" {
                       "source $env(TCLTKGRASSBASE)/module/v.what"
                }
		"�������� ����� � �������" "" {
                       "source $env(TCLTKGRASSBASE)/module/d.measure"
                }
        }
        "�������������" "" {
            "� �����" "" {
                "source $env(TCLTKGRASSBASE)/module/v.to.rast"
            }
            "� �������" "" {
                "source $env(TCLTKGRASSBASE)/module/v.to.sites"
            }
        }
        "��������" "" {
            "�������/����������� ���������" "" {
                "source $env(TCLTKGRASSBASE)/module/v.support_option=build"
            }
            "������������� ���������" "" {
                "source $env(TCLTKGRASSBASE)/module/v.support_option=edit"
            }
            "���������� �����" "" {
                "source $env(TCLTKGRASSBASE)/module/v.prune"
            }
            "�������� ���������" "" {
                "source $env(TCLTKGRASSBASE)/module/v.spag"
            }
            "�������� ��������� ASCII �����" "" {
                "source $env(TCLTKGRASSBASE)/module/v.ascii.spag"
            }
            "������� ������� �����" "" {
                "source $env(TCLTKGRASSBASE)/module/v.clean"
            }
            "������� ����������� ����" "" {
                "source $env(TCLTKGRASSBASE)/module/v.trim"
            }
	    "��������� ��������� ���������" "" {
                "source $env(TCLTKGRASSBASE)/module/v.build.polylines"
            } 
            -separator
            "�������� �������� ASCII ��������" "" {
                "source $env(TCLTKGRASSBASE)/module/v.proj"
            }
            "������������� ���������� ASCII ��������" "" {
                "source $env(TCLTKGRASSBASE)/module/v.transform"
            }
            "������ ASCII �������� � GRASS �������" "" {
                "run v.import &"
            }
            "������� GRASS �������� � ASCII �������" "" {
                "run v.export &"
            }
            -separator
            "����������" "" {
                "run v.digspline &"
            }
            "������� �����" "" {
                "source $env(TCLTKGRASSBASE)/module/v.mkgrid"
            }
            "�������� (������� ����� ��������)" "" {
                "source $env(TCLTKGRASSBASE)/module/v.cutter"
            }
            "��������� ��������� �����" "" {
                "source $env(TCLTKGRASSBASE)/module/v.patch"
            }
            "�������������� ����������" "" {
                 "source $env(TCLTKGRASSBASE)/module/v.geom"
            }
	    "������������ ��������� ���� �� ��������� �����" "" {
                "source $env(TCLTKGRASSBASE)/module/v.surf.rst"
            }
        }
        "������" "" {
            "������� ����������" "" {
                "source $env(TCLTKGRASSBASE)/module/v.info"
            }
            "����� ����������" "" {
                "source $env(TCLTKGRASSBASE)/module/v.stats"
            }
            "�������" "" {
                "source $env(TCLTKGRASSBASE)/module/v.report"
            }
        }
    }
    ������� "������ ���� ��������" {
        �������� "" $display_sites
        "������ �����" "" {
            "source $env(TCLTKGRASSBASE)/module/d.what.sites"
        }
	"������ �������" "" {
	    	"source $env(TCLTKGRASSBASE)/module/s.mask"
	}
	"�����. �������� ����. �������� �� ������� ������" "" {
	    	"source $env(TCLTKGRASSBASE)/module/s.windavg"
	}
 
        ������������ "" {
            "IDW" "" {
                "source $env(TCLTKGRASSBASE)/module/s.surf.idw"
            }
            "������" "" {
                 "source $env(TCLTKGRASSBASE)/module/s.surf.rst"
            }
        }
	"������������� �����" "" {
            "� �����" "" {
                "source $env(TCLTKGRASSBASE)/module/s.to.rast"
            }
            "� �������" "" {
                "source $env(TCLTKGRASSBASE)/module/s.to.vect"
            }
        }
        "������" "" {
            "�������� ����������" "" {
                "source $env(TCLTKGRASSBASE)/module/s.info"
            }
        }
    }
    ������ "��������� �������" $image_processing
    ������ "������ ���� � GRASS" {
        "�����" "" {
            "ESRI ARC/INFO ASCII-GRID" "" {
                "source $env(TCLTKGRASSBASE)/module/r.in.arc"
            }
            "TIFF 8bit" "" {
                "source $env(TCLTKGRASSBASE)/module/r.in.tiff"
            }
	    "PNG (24bit)" "" {
                "source $env(TCLTKGRASSBASE)/module/r.in.png"
            }
            "PPM (24bit)" "" {
                "source $env(TCLTKGRASSBASE)/module/r.in.ppm"
            }
            "HDF" "" {
                "source $env(TCLTKGRASSBASE)/module/r.in.hdf"
            }
            "ERDAS LAN" "" {
                "source $env(TCLTKGRASSBASE)/module/i.in.erdas"
            }
	    "������ ������� (��������� GDAL)" "" {
                "source $env(TCLTKGRASSBASE)/module/r.in.gdal"
            }
            -separator
            "�������� ���� GTOPO30 � ���/���� ������" "" {
                "source $env(TCLTKGRASSBASE)/module/r.in.bin"
            }
            "�������� ���� GTOPO30 � UTM" "" {
                "source $env(TCLTKGRASSBASE)/module/r.in.ll"
            }
        }
        "�������" "" {
            "ASCII GRASS �������" "" {
                "source $env(TCLTKGRASSBASE)/module/v.in.ascii"
            }
            "����� �� ARC/INFO ungenerate " "" {
                "source $env(TCLTKGRASSBASE)/module/v.in.arc"
            }
	    "ESRI shapefile" "" {
                "source $env(TCLTKGRASSBASE)/module/v.in.shape"
            }
	    "AUTOCAD DXF �����" "" {
                "source $env(TCLTKGRASSBASE)/module/v.in.dxf"
            }
            "������ �������" "" {
                "run v.import &"
            }
	    "Garmin GPS �����.�����/��������/�������" "" {
		"source $env(TCLTKGRASSBASE)/module/v.in.garmin"
	    }
        }
        "�������" "" {
            "ASCII/SPOT �����" "" {
                "source $env(TCLTKGRASSBASE)/module/s.in.ascii"
            }
            "Garmin GPS �����.�����/��������/�������" "" {
		"source $env(TCLTKGRASSBASE)/module/s.in.garmin"
	    }
        }
    }
    ������� "������� ���� �� GRASS" {
        "�����" "" {
            "TIFF (8/24bit)" "" {
                "source $env(TCLTKGRASSBASE)/module/r.out.tiff"
            }
	    "Binary file" "" {
                "source $env(TCLTKGRASSBASE)/module/r.out.bin"
            }
	    "ESRI ARC/INFO ASCII-GRID" "" {
                "source $env(TCLTKGRASSBASE)/module/r.out.arc"
            }
            "PPM (24bit)" "" {
                "source $env(TCLTKGRASSBASE)/module/r.out.ppm"
            }
            "HDF" "" {
                "source $env(TCLTKGRASSBASE)/module/r.out.hdf"
            }
	    "GRASS ASCII" "" {
                "source $env(TCLTKGRASSBASE)/module/r.out.ascii"
            }
            "ERDAS/LAN" "" {
                "source $env(TCLTKGRASSBASE)/module/i.out.erdas"
            }
            "GRASS CELL � TIFF" "" {
                "source $env(TCLTKGRASSBASE)/module/cell.out.tiff"
            }
        }
        "�������" "" {
            "ASCII GRASS �������" "" {
                "source $env(TCLTKGRASSBASE)/module/v.out.ascii"
            }
	    "ARC/INFO E00 file" "" {
                "source $env(TCLTKGRASSBASE)/module/v.out.e00"
            }
            "� ���� ARC/INFO ungenerate file" "" {
                "source $env(TCLTKGRASSBASE)/module/v.out.arc"
            }
            "������ �������" "" {
                "run v.export &"
            }
        }
        "�������" "" {
            "ASCII ����" "" {
                "source $env(TCLTKGRASSBASE)/module/s.out.ascii"
            }
        }
    }
    �������� "�������� ����" {
        "������� paint" "" {
            "�������/���. ������" "" {
                "run p.icons &"
            }
            "�������/���. �����" "" {
                "run p.labels &"
            }
            "�������� �����" "" {
                "source $env(TCLTKGRASSBASE)/module/d.paint.labels"
            }
            "����� ��������" "" {
                "source $env(TCLTKGRASSBASE)/module/p.select"
            }
            "�������� ����� paint" "" {
                "source $env(TCLTKGRASSBASE)/module/p.map.new"
            }
        }
        "PostScript" "" {
            "�������/���. ������" "" {
                 "source $env(TCLTKGRASSBASE)/module/ps.icon"
             }
             "����� ��������" "" {
                 "source $env(TCLTKGRASSBASE)/module/ps.select"
             }
             "�������� ����� PostScript" "" {
                 "source $env(TCLTKGRASSBASE)/module/ps.map"
             }
        }
        "Xfig (�����.)" "" {
            "run xfig&"
        }
    }
    �� "���� ������" {
        "PostgreSQL" "" {
            "�����" "" {
                "����� DB" "" {
                    "source $env(TCLTKGRASSBASE)/module/g.select.pg"
                }
    	    "������ ������" "" {
                    "source $env(TCLTKGRASSBASE)/module/g.table.pg"
                }
            "������ ��������" "" {
                    "source $env(TCLTKGRASSBASE)/module/g.column.pg"
                }
	    "����������" "" {
                    "source $env(TCLTKGRASSBASE)/module/g.stats.pg"
                }
            }
            "������ �����" "" {
                "�������" "" {
                    "source $env(TCLTKGRASSBASE)/module/d.what.v.pg"
                }
	        "�������" "" {
                    "source $env(TCLTKGRASSBASE)/module/d.what.s.pg"
                }
	        "�����" "" {
                    "source $env(TCLTKGRASSBASE)/module/d.what.r.pg"
                }
            }
	    "��������" "" {
                "�������" "" {
                    "source $env(TCLTKGRASSBASE)/module/d.vect.pg"
                }
	        "�������" "" {
                    "source $env(TCLTKGRASSBASE)/module/d.site.pg"
                }
	        "�����" "" {
                    "source $env(TCLTKGRASSBASE)/module/d.rast.pg"
                }
	    }
            "������� ��������" "" {
                    "source $env(TCLTKGRASSBASE)/module/v.reclass.pg"
            }   
        }
	-separator
	"DBMI" "" {
            "������� �������" "" {
                "source $env(TCLTKGRASSBASE)/module/db.connect.driver"
            }
            "�����������" "" {
                "source $env(TCLTKGRASSBASE)/module/db.connect"
            }
            -separator
	    "������ ������" "" {
                "source $env(TCLTKGRASSBASE)/module/db.tables"
            }	    	    
            "������ ��������" "" {
                "source $env(TCLTKGRASSBASE)/module/db.columns"
            }
            "�������� ������" "" {
                "source $env(TCLTKGRASSBASE)/module/db.describe"
            }
            -separator
            "������� ���" "" {
                "source $env(TCLTKGRASSBASE)/module/db.select.all"
            }	    
            "�������" "" {
                "source $env(TCLTKGRASSBASE)/module/db.select"
            } 
            "���������" "" {
                "source $env(TCLTKGRASSBASE)/module/db.execute"
            }
	    -separator
            "������� ��������" "" {
                "source $env(TCLTKGRASSBASE)/module/v.db.reclass"
            }
            "��������� ������� � ��" "" {
                "source $env(TCLTKGRASSBASE)/module/v.to.db"
            }	    	    
        }
    }
    ������ "������ ��������������" $misc
    ������ ������ {
        "������" "" {
           "source $env(TCLTKGRASSBASE)/module/g.manual"
        }
        -separator
        "������" "" {
           "source $env(TCLTKGRASSBASE)/main/help.tcl"
        }
        "� ���������" "" {
           "source $env(TCLTKGRASSBASE)/main/about.tcl"
        }
	"� GRASS" "" {
           "source $env(TCLTKGRASSBASE)/main/grassabout.tcl"
        }
	-separator
	"�� ���� �������" "" {
	   "exec $env(TCLTKGRASSBASE)/main/tksys.tcl --tcltk"
	}
	-separator
	"������ �� ��������" "" {
	    "source $env(TCLTKGRASSBASE)/main/help-scripting.tcl"
	}
	"������ �� html-browser" "" {
	    "source $env(TCLTKGRASSBASE)/main/help-netscape.tcl"
	}
    }
    ����� ����! quit
}
