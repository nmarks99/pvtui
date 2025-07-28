#include "motor_display.hpp"
#include "ftxui/component/component.hpp"
#include "pvtui.hpp"
#include <ftxui/dom/elements.hpp>
    
ftxui::Decorator ColorDisabled = bgcolor(ftxui::Color::RGBA(80,10,4,230)) | color(ftxui::Color::Black);

SmallMotorDisplay::SmallMotorDisplay(PVGroup &pvgroup, const pvtui::ArgParser &args)
    : DisplayBase(pvgroup), args(args),
    desc(pvgroup, args, "$(P)$(M).DESC", pvtui::PVPutType::String),
    stop(pvgroup, args, "$(P)$(M).STOP", " STOP "),
    use_set(pvgroup, args, "$(P)$(M).SET"),
    en_dis(pvgroup, args, "$(P)$(M)_able"),
    egu(pvgroup, args, "$(P)$(M).EGU"),
    hls(pvgroup, args, "$(P)$(M).HLS"),
    lls(pvgroup, args, "$(P)$(M).LLS"),
    dmov(pvgroup, args, "$(P)$(M).DMOV"),
    rbv(pvgroup, args, "$(P)$(M).RBV"),
    twf(pvgroup, args, "$(P)$(M).TWF", " > "),
    twr(pvgroup, args, "$(P)$(M).TWR", " < "),
    twv(pvgroup, args, "$(P)$(M).TWV", pvtui::PVPutType::Double),
    val(pvgroup, args, "$(P)$(M).VAL", pvtui::PVPutType::Double)
{}

ftxui::Component SmallMotorDisplay::get_container() {
    using namespace ftxui;
    return Container::Vertical({
	desc.component(),
	val.component(),
	Container::Horizontal({
	    twr.component(),
	    twv.component(),
	    twf.component(),
	}),
	Container::Horizontal({
	    use_set.component(), stop.component()
	})
    });
}

ftxui::Element SmallMotorDisplay::get_renderer() {
    using namespace ftxui;
    using namespace pvtui;
    return ftxui::vbox({

	desc.component()->Render() 
	    | EPICSColor::EDIT
	    | size(WIDTH, EQUAL, 20)
	    | underlined
	    | center,

	separatorEmpty(),

	hbox({
	    filler() | size(WIDTH, EQUAL, egu.value().size()+1),
	    text(lls.value() ? unicode::rectangle(1) : "  ") | color(Color::Red),
	    separatorEmpty(), separatorEmpty(),
	    text(rbv.value()) | (use_set.value().index==0 ? EPICSColor::READBACK : color(Color::Yellow2)),
	    separatorEmpty(), separatorEmpty(),
	    text(hls.value() ? unicode::rectangle(1) : "  ") | color(Color::Red),
	    separatorEmpty(),
	    text(egu.value()) | color(Color::Black)
	}) | center,

	hbox({
	    val.component()->Render()
		| (en_dis.value().index==0 ? EPICSColor::EDIT : ColorDisabled)
		| size(WIDTH, EQUAL, 10)
		| (dmov.value() == 0 ? borderHeavy | color(Color::Green) : borderEmpty),
	}) | center,

	hbox({
	    twr.component()->Render() | color(Color::Black),
	    separatorEmpty(),
	    twv.component()->Render() | EPICSColor::EDIT | size(WIDTH, EQUAL, 10),
	    separatorEmpty(),
	    twf.component()->Render() | color(Color::Black),
	}) | center,
	separatorEmpty(),
	hbox({
	    use_set.component()->Render() | EPICSColor::EDIT,
	    separatorEmpty(),
	    separatorEmpty(),
	    stop.component()->Render() | color(Color::Red) | bold,
	}) | center,
	separatorEmpty()

    })
    | size(WIDTH, EQUAL, 24)
    | border 
    | color(Color::Black) 
    | center 
    | bgcolor(Color::RGB(196,196,196));
}


MediumMotorDisplay::MediumMotorDisplay(PVGroup &pvgroup, const pvtui::ArgParser &args)
    : DisplayBase(pvgroup), args(args),
    desc(pvgroup, args, "$(P)$(M).DESC", pvtui::PVPutType::String),
    use_set(pvgroup, args, "$(P)$(M).SET"),
    en_dis(pvgroup, args, "$(P)$(M)_able"),
    egu(pvgroup, args, "$(P)$(M).EGU"),
    hls(pvgroup, args, "$(P)$(M).HLS"),
    lls(pvgroup, args, "$(P)$(M).LLS"),
    dmov(pvgroup, args, "$(P)$(M).DMOV"),
    rbv(pvgroup, args, "$(P)$(M).RBV"),
    drbv(pvgroup, args, "$(P)$(M).DRBV"),
    twf(pvgroup, args, "$(P)$(M).TWF", " > "),
    twr(pvgroup, args, "$(P)$(M).TWR", " < "),
    twv(pvgroup, args, "$(P)$(M).TWV", pvtui::PVPutType::Double),
    val(pvgroup, args, "$(P)$(M).VAL", pvtui::PVPutType::Double),
    dval(pvgroup, args, "$(P)$(M).DVAL", pvtui::PVPutType::Double),
    able(pvgroup, args, "$(P)$(M)_able"),
    spmg(pvgroup, args, "$(P)$(M).SPMG"),
    llm(pvgroup, args, "$(P)$(M).LLM", pvtui::PVPutType::Double),
    hlm(pvgroup, args, "$(P)$(M).HLM", pvtui::PVPutType::Double),
    dllm(pvgroup, args, "$(P)$(M).DLLM", pvtui::PVPutType::Double),
    dhlm(pvgroup, args, "$(P)$(M).DHLM", pvtui::PVPutType::Double)
{}

ftxui::Component MediumMotorDisplay::get_container() {
    using namespace ftxui;
    return Container::Vertical({
	desc.component(),
	Container::Horizontal({
	    Container::Vertical({
		hlm.component(),
		val.component(),
		llm.component(),
	    }),
	    Container::Vertical({
		dhlm.component(),
		dval.component(),
		dllm.component(),
	    }),
	    Container::Vertical({
		spmg.component(),
	    }),
	}),
	Container::Horizontal({
	    twr.component(), twv.component(), twf.component()
	}),
	Container::Horizontal({
	    able.component(), use_set.component(),
	})
    });

}

ftxui::Element MediumMotorDisplay::get_renderer() {
    using namespace ftxui;
    using namespace pvtui;

    auto &desc_pv = pvgroup.get_pv(desc.pv_name());
    return ftxui::vbox({
	(desc_pv.connected() ? text("") : text("Disconnected") | color(Color::Red)) | center,

	desc.component()->Render() | color(Color::Black) | bgcolor(Color::RGB(210,210,210)) | size(WIDTH, EQUAL, 22) | center,
	separatorEmpty(),

	// 6 column hbox of vbox's
	// none | none | user | dial | lims/egu | spmg
	hbox({
	    filler() | size(WIDTH, EQUAL, egu.value().length()+8),
	    vbox({
		text("User") | center,
		hlm.component()->Render()
		    | center
		    | EPICSColor::EDIT
		    | size(WIDTH, EQUAL, 10),
		text(rbv.value())
		    | (use_set.value().index==0 ? EPICSColor::READBACK : color(Color::Yellow2))
		    | (dmov.value() == 0 ? borderHeavy | color(Color::Green) : borderEmpty)
		    | center,
		val.component()->Render()
		    | center
		    | (able.value().index==0 ? EPICSColor::EDIT : ColorDisabled)
		    | size(WIDTH, EQUAL, 10) | size(HEIGHT, EQUAL, 2),
		separatorEmpty(), 	
		llm.component()->Render()
		    | EPICSColor::EDIT
		    | size(WIDTH, EQUAL, 10),
	    }),
	    separatorEmpty(), 	
	    vbox({
		text("Dial") | center,
		dhlm.component()->Render()
		    | center
		    | EPICSColor::EDIT
		    | size(WIDTH, EQUAL, 10),
		text(drbv.value())
		    | (use_set.value().index==0 ? EPICSColor::READBACK : color(Color::Yellow2))
		    | (dmov.value() == 0 ? borderHeavy | color(Color::Green) : borderEmpty)
		    | center,
		dval.component()->Render()
		    | center
		    | (able.value().index==0 ? EPICSColor::EDIT : ColorDisabled)
		    | size(WIDTH, EQUAL, 10)
		    | size(HEIGHT, EQUAL, 2),
		separatorEmpty(), 	
		dllm.component()->Render()
		    | EPICSColor::EDIT
		    | size(WIDTH, EQUAL, 10),
	    }),
	    separatorEmpty(),
	    vbox({
		separatorEmpty(),
		text(hls.value() ? unicode::rectangle(2) : "") | color(Color::Red),
		filler(),
		text(egu.value()) | EPICSColor::READBACK,
		separatorEmpty(),
		text(lls.value() ? unicode::rectangle(2) : "") | color(Color::Red),
	    }) | size(WIDTH, EQUAL, egu.value().length()),
	    separatorEmpty(),
	    spmg.component()->Render()
		| EPICSColor::EDIT
		| size(HEIGHT, EQUAL, 2),
	}) | center,
	
	separatorEmpty(), 	

	hbox({
	    twr.component()->Render() | color(Color::Black),
	    separatorEmpty(),
	    twv.component()->Render()
		| center
		| EPICSColor::EDIT
		| size(WIDTH, EQUAL, 11),
	    separatorEmpty(),
	    twf.component()->Render() | color(Color::Black),
	}) | center,

	separatorEmpty(),

	hbox({
	    separatorEmpty(),
	    use_set.component()->Render() | EPICSColor::EDIT,
	    separatorEmpty(),
	    able.component()->Render() | EPICSColor::EDIT,
	}) | center,
	separatorEmpty(),

    })| center | EPICSColor::BACKGROUND;
}

AllMotorDisplay::AllMotorDisplay(PVGroup &pvgroup, const pvtui::ArgParser &args)
    : DisplayBase(pvgroup), args(args),
    desc(pvgroup, args, "$(P)$(M).DESC", pvtui::PVPutType::String),
    use_set(pvgroup, args, "$(P)$(M).SET"),
    en_dis(pvgroup, args, "$(P)$(M)_able"),
    egu(pvgroup, args, "$(P)$(M).EGU", pvtui::PVPutType::String),
    hls(pvgroup, args, "$(P)$(M).HLS"),
    lls(pvgroup, args, "$(P)$(M).LLS"),
    dmov(pvgroup, args, "$(P)$(M).DMOV"),
    rbv(pvgroup, args, "$(P)$(M).RBV"),
    rrbv(pvgroup, args, "$(P)$(M).RRBV"),
    drbv(pvgroup, args, "$(P)$(M).DRBV"),
    twf(pvgroup, args, "$(P)$(M).TWF", " > "),
    twr(pvgroup, args, "$(P)$(M).TWR", " < "),
    twv(pvgroup, args, "$(P)$(M).TWV", pvtui::PVPutType::Double),
    val(pvgroup, args, "$(P)$(M).VAL", pvtui::PVPutType::Double),
    rval(pvgroup, args, "$(P)$(M).RVAL", pvtui::PVPutType::Double),
    rlv(pvgroup, args, "$(P)$(M).RLV", pvtui::PVPutType::Double),
    dval(pvgroup, args, "$(P)$(M).DVAL", pvtui::PVPutType::Double),
    prec(pvgroup, args, "$(P)$(M).PREC", pvtui::PVPutType::Int),
    rtry(pvgroup, args, "$(P)$(M).RTRY", pvtui::PVPutType::Int),
    off(pvgroup, args, "$(P)$(M).OFF", pvtui::PVPutType::Double),
    vmax(pvgroup, args, "$(P)$(M).VMAX", pvtui::PVPutType::Double),
    vbas(pvgroup, args, "$(P)$(M).VBAS", pvtui::PVPutType::Double),
    velo(pvgroup, args, "$(P)$(M).VELO", pvtui::PVPutType::Double),
    accl(pvgroup, args, "$(P)$(M).ACCL", pvtui::PVPutType::Double),
    mres(pvgroup, args, "$(P)$(M).MRES", pvtui::PVPutType::Double),
    eres(pvgroup, args, "$(P)$(M).ERES", pvtui::PVPutType::Double),
    rres(pvgroup, args, "$(P)$(M).RRES", pvtui::PVPutType::Double),
    able(pvgroup, args, "$(P)$(M)_able"),
    spmg(pvgroup, args, "$(P)$(M).SPMG"),
    foff(pvgroup, args, "$(P)$(M).FOFF"),
    cnen(pvgroup, args, "$(P)$(M).CNEN"),
    dir(pvgroup, args, "$(P)$(M).DIR"),
    urip(pvgroup, args, "$(P)$(M).URIP"),
    ueip(pvgroup, args, "$(P)$(M).UEIP"),
    llm(pvgroup, args, "$(P)$(M).LLM", pvtui::PVPutType::Double),
    hlm(pvgroup, args, "$(P)$(M).HLM", pvtui::PVPutType::Double),
    dllm(pvgroup, args, "$(P)$(M).DLLM", pvtui::PVPutType::Double),
    dhlm(pvgroup, args, "$(P)$(M).DHLM", pvtui::PVPutType::Double)
{}

ftxui::Component AllMotorDisplay::get_container() {
    using namespace ftxui;
    return Container::Vertical({
	desc.component(),

	Container::Horizontal({
	    Container::Vertical({
		hlm.component(),
		val.component(),
		llm.component(),
		rlv.component(),
	    }),
	    Container::Vertical({
		dhlm.component(),
		dval.component(),
		dllm.component(),
	    }),
	    rval.component(),
	    able.component(),
	    spmg.component(),
	}),
	Container::Horizontal({
	    twr.component(),
	    twv.component(),
	    twf.component(),
	}),
	Container::Horizontal({
	    Container::Vertical({
		vmax.component(),
		velo.component(),
		vbas.component(),
		accl.component(),
	    }),
	    Container::Vertical({
		foff.component(),
		use_set.component(),
		off.component(),
		dir.component(),
		egu.component(),
	    })
	}),
	Container::Horizontal({
	    Container::Vertical({
		mres.component(),
		eres.component(),
		rres.component(),
		rtry.component(),
	    }),
	    Container::Vertical({
		ueip.component(),
		urip.component(),
		prec.component(),
		cnen.component(),
	    })
	}),
    });

}

ftxui::Element AllMotorDisplay::get_renderer() {
    using namespace ftxui;
    using namespace pvtui;

    // use DESC pv to check connection status
    auto &conn_pv = pvgroup.get_pv(desc.pv_name());

    auto drive = vbox({
	hbox({
	    vbox({
		separatorEmpty(), 	
		text("Hi Limit: "),
		text("Readback: "),
		text("Absolute: "),
		text("Lo Limit: "),
		text("Relative: "),
	    }) | color(Color::Black),
	    vbox({
		text("User") | center | color(Color::Black),
		hlm.component()->Render()
		    | center
		    | EPICSColor::EDIT
		    | size(WIDTH, EQUAL, 10),
		text(rbv.value())
		    | (use_set.value().index==0 ? EPICSColor::READBACK : color(Color::Yellow2))
		    | center,
		val.component()->Render()
		    | center
		    | (able.value().index==0 ? EPICSColor::EDIT : ColorDisabled)
		    | size(WIDTH, EQUAL, 10),
		llm.component()->Render()
		    | EPICSColor::EDIT
		    | size(WIDTH, EQUAL, 10),
		rlv.component()->Render()
		    | EPICSColor::EDIT
		    | size(WIDTH, EQUAL, 10),
	    }),
	    separatorEmpty(), 	
	    vbox({
		text("Dial") | center | color(Color::Black),
		dhlm.component()->Render()
		    | center
		    | EPICSColor::EDIT
		    | size(WIDTH, EQUAL, 10),
		text(drbv.value())
		    | (use_set.value().index==0 ? EPICSColor::READBACK : color(Color::Yellow2))
		    | center,
		dval.component()->Render()
		    | center
		    | (able.value().index==0 ? EPICSColor::EDIT : ColorDisabled)
		    | size(WIDTH, EQUAL, 10),
		dllm.component()->Render()
		    | EPICSColor::EDIT
		    | size(WIDTH, EQUAL, 10),
		filler() | size(WIDTH, EQUAL, 10),
	    }),
	    separatorEmpty(),
	    vbox({
		text("Raw") | center | color(Color::Black),
		hls.value() == 1 ? text(unicode::rectangle(2)) | color(Color::Red) : text(""),
		text(rrbv.value()) | EPICSColor::READBACK,
		rval.component()->Render() | EPICSColor::EDIT | size(WIDTH, EQUAL, 6),
		lls.value() == 1 ? text(unicode::rectangle(2)) | color(Color::Red) : text(""),
	    }),
	    separatorEmpty(),
	    vbox({
		dmov.value()==0 ? text("Moving")
		    | color(Color::Green)
		    | bgcolor(Color::Black)
		    | bold | italic : text(""),
		filler(),
		able.component()->Render() | EPICSColor::EDIT | size(WIDTH, EQUAL, 7)
	    }),
	    separatorEmpty(),
	    vbox({
		filler(),
		spmg.component()->Render() | EPICSColor::EDIT | size(WIDTH, EQUAL, 7)
	    }),
	}),
	hbox({
	    text("Twk:") | color(Color::Black),
	    twr.component()->Render() | color(Color::Black),
	    separatorEmpty(),
	    twv.component()->Render() | EPICSColor::EDIT | size(WIDTH, EQUAL, 10),
	    separatorEmpty(),
	    twf.component()->Render() | color(Color::Black),
	}),
    });

    auto dynamics = vbox({
	hbox({
	    text(" Max Speed: ") | color(Color::Black),
	    separatorEmpty(),
	    vmax.component()->Render() | EPICSColor::EDIT | size(WIDTH, GREATER_THAN, 7),
	}),

	hbox({
	    text("     Speed: ") | color(Color::Black),
	    separatorEmpty(),
	    velo.component()->Render() | EPICSColor::EDIT | size(WIDTH, GREATER_THAN, 7),
	}),

	hbox({
	    text(" Min Speed: ") | color(Color::Black),
	    separatorEmpty(),
	    vbas.component()->Render() | EPICSColor::EDIT | size(WIDTH, GREATER_THAN, 7),
	}),

	hbox({
	    text("     Accel: ") | color(Color::Black),
	    separatorEmpty(),
	    accl.component()->Render() | EPICSColor::EDIT | size(WIDTH, GREATER_THAN, 7),
	}),
    }) | size(WIDTH, EQUAL, 26);

    auto calibration = vbox({
	hbox({
	    text("Off: ") | color(Color::Black),
	    off.component()->Render() | EPICSColor::EDIT | size(WIDTH, GREATER_THAN, 7),
	    separatorEmpty(),
	    foff.component()->Render() | EPICSColor::EDIT | size(WIDTH, GREATER_THAN, 7),
	}),
	hbox({
	    text("Cal: ") | color(Color::Black),
	    use_set.component()->Render() | EPICSColor::EDIT | size(WIDTH, EQUAL, 7),
	}),
	hbox({
	    text("Dir: ") | color(Color::Black),
	    dir.component()->Render() | EPICSColor::EDIT | size(WIDTH, EQUAL, 7),
	}),
	hbox({
	    text("EGU: ") | color(Color::Black),
	    egu.component()->Render() | EPICSColor::EDIT | size(WIDTH, EQUAL, 7),
	}),
    }) | size(WIDTH, EQUAL, 26);

    auto res_left = vbox({
	hbox({
	    text("   Motor res: ") | color(Color::Black),
	    mres.component()->Render() | EPICSColor::EDIT | size(WIDTH, GREATER_THAN, 7),
	}),
	hbox({
	    text(" Encoder res: ") | color(Color::Black),
	    eres.component()->Render() | EPICSColor::EDIT | size(WIDTH, GREATER_THAN, 7),
	}),
	hbox({
	    text("Readback res: ") | color(Color::Black),
	    rres.component()->Render() | EPICSColor::EDIT | size(WIDTH, GREATER_THAN, 7),
	}),
	hbox({
	    text("     Retries: ") | color(Color::Black),
	    rtry.component()->Render() | EPICSColor::EDIT | size(WIDTH, GREATER_THAN, 7),
	}),
    }) | size(WIDTH, EQUAL, 26) | color(Color::Black);

    auto res_right= vbox({
	hbox({
	    text("   Precision: ") | color(Color::Black),
	    prec.component()->Render() | EPICSColor::EDIT | size(WIDTH, EQUAL, 6),
	}),
	hbox({
	    text(" Use encoder: ") | color(Color::Black),
	    ueip.component()->Render() | EPICSColor::EDIT | size(WIDTH, EQUAL, 6),
	}),
	hbox({
	    text("Use readback: ") | color(Color::Black),
	    urip.component()->Render() | EPICSColor::EDIT | size(WIDTH, EQUAL, 6),
	}),
	separatorEmpty(),
	hbox({
	    text("Torque: ") | color(Color::Black),
	    cnen.component()->Render() | EPICSColor::EDIT | size(WIDTH, EQUAL, 14),
	}),
    }) | size(WIDTH, EQUAL, 26) | color(Color::Black);

    auto title = hbox({
	desc.component()->Render() | EPICSColor::EDIT | size(WIDTH, EQUAL, 26),
	filler(),
	text("(" + args.macros.at("P") + args.macros.at("M") + ")")
	    | color(Color::Black)
	    | bold
	    | xflex,
    });

    return vbox({
	title,
	separator() | color(Color::Black),
	drive,
	separator() | color(Color::Black),
	hbox({
	    dynamics,
	    separator() | color(Color::Black),
	    calibration
	}),
	separator() | color(Color::Black),
	hbox({
	    res_left,
	    separator() | color(Color::Black),
	    res_right
	}),
	separatorEmpty(),
	!conn_pv.connected() ? text("Disconnected") | color(Color::Red) | bold | center : text(""),
    }) | size(WIDTH, EQUAL, 52);
}
