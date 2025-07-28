#include "motor_display.hpp"
#include "ftxui/component/component.hpp"
#include "pvtui.hpp"
#include <ftxui/dom/elements.hpp>
    
ftxui::Decorator ColorDisabled = bgcolor(ftxui::Color::RGBA(80,10,4,230)) | color(ftxui::Color::Black);

void SmallMotorDisplay::init() {
    using namespace pvtui;

    connect_pv(desc, args.replace("$(P)$(M).DESC"), MonitorOn);
    desc.set_component(PVInput(pvgroup.get_pv(desc.pv_name), desc.value, PVPutType::String));
    
    connect_pv(val, args.replace("$(P)$(M).VAL"), MonitorOn);
    val.set_component(PVInput(pvgroup.get_pv(val.pv_name), val.value, PVPutType::Double));

    connect_pv(twr, args.replace("$(P)$(M).TWR"), MonitorOff);
    twr.set_component(PVButton(pvgroup.get_pv(twr.pv_name), " < ", 1));

    connect_pv(twv, args.replace("$(P)$(M).TWV"), MonitorOn);
    twv.set_component(PVInput(pvgroup.get_pv(twv.pv_name), twv.value, PVPutType::Double));

    connect_pv(twf, args.replace("$(P)$(M).TWF"), MonitorOff);
    twf.set_component(PVButton(pvgroup.get_pv(twf.pv_name), " > ", 1));

    connect_pv(rbv, args.replace("$(P)$(M).RBV"), MonitorOn);

    connect_pv(egu, args.replace("$(P)$(M).EGU"), MonitorOn);

    connect_pv(hls, args.replace("$(P)$(M).HLS"), MonitorOn);

    connect_pv(lls, args.replace("$(P)$(M).LLS"), MonitorOn);

    connect_pv(en_dis, args.replace("$(P)$(M)_able"), MonitorOn);

    connect_pv(use_set, args.replace("$(P)$(M).SET"), MonitorOn);
    use_set.set_component(PVChoiceH(pvgroup.get_pv(use_set.pv_name), use_set.value.choices, use_set.value.index));

    connect_pv(dmov, args.replace("$(P)$(M).DMOV"), MonitorOn);

    connect_pv(stop, args.replace("$(P)$(M).STOP"), MonitorOff);
    stop.set_component(PVButton(pvgroup.get_pv(stop.pv_name), " STOP ", 1));
}

SmallMotorDisplay::SmallMotorDisplay(PVGroup &pvgroup, const pvtui::ArgParser &args)
    : DisplayBase(pvgroup), args(args) {
    this->init();
}

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
	    filler() | size(WIDTH, EQUAL, egu.value.size()+1),
	    text(lls.value ? unicode::rectangle(1) : "  ") | color(Color::Red),
	    separatorEmpty(), separatorEmpty(),
	    text(rbv.value) | (use_set.value.index==0 ? EPICSColor::READBACK : color(Color::Yellow2)),
	    separatorEmpty(), separatorEmpty(),
	    text(hls.value ? unicode::rectangle(1) : "  ") | color(Color::Red),
	    separatorEmpty(),
	    text(egu.value) | color(Color::Black)
	}) | center,

	hbox({
	    val.component()->Render()
		| (en_dis.value.index==0 ? EPICSColor::EDIT : ColorDisabled)
		| size(WIDTH, EQUAL, 10)
		| (dmov.value == 0 ? borderHeavy | color(Color::Green) : borderEmpty),
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


void MediumMotorDisplay::init() {
    using namespace pvtui;

    connect_pv(desc, args.replace("$(P)$(M).DESC"), MonitorOn);
    desc.set_component(PVInput(pvgroup.get_pv(desc.pv_name), desc.value, PVPutType::String));
    
    connect_pv(val, args.replace("$(P)$(M).VAL"), MonitorOn);
    val.set_component(PVInput(pvgroup.get_pv(val.pv_name), val.value, PVPutType::Double));

    connect_pv(twr, args.replace("$(P)$(M).TWR"), MonitorOff);
    twr.set_component(PVButton(pvgroup.get_pv(twr.pv_name), " < ", 1));
    
    connect_pv(twv, args.replace("$(P)$(M).TWV"), MonitorOn);
    twv.set_component(PVInput(pvgroup.get_pv(twv.pv_name), twv.value, PVPutType::Double));

    connect_pv(twf, args.replace("$(P)$(M).TWF"), MonitorOff);
    twf.set_component(PVButton(pvgroup.get_pv(twf.pv_name), " > ", 1));

    connect_pv(rbv, args.replace("$(P)$(M).RBV"), MonitorOn);

    connect_pv(egu, args.replace("$(P)$(M).EGU"), MonitorOn);

    connect_pv(hls, args.replace("$(P)$(M).HLS"), MonitorOn);
    
    connect_pv(lls, args.replace("$(P)$(M).LLS"), MonitorOn);

    connect_pv(use_set, args.replace("$(P)$(M).SET"), MonitorOn);
    use_set.set_component(PVChoiceH(pvgroup.get_pv(use_set.pv_name), use_set.value.choices, use_set.value.index));

    connect_pv(dmov, args.replace("$(P)$(M).DMOV"), MonitorOn);

    connect_pv(spmg, args.replace("$(P)$(M).SPMG"), MonitorOn);
    spmg.set_component(PVChoiceV(pvgroup.get_pv(spmg.pv_name), spmg.value.choices, spmg.value.index));

    connect_pv(able, args.replace("$(P)$(M)_able"), MonitorOn);
    able.set_component(PVChoiceH(pvgroup.get_pv(able.pv_name), able.value.choices, able.value.index));

    connect_pv(dval, args.replace("$(P)$(M).DVAL"), MonitorOn);
    dval.set_component(PVInput(pvgroup.get_pv(dval.pv_name), dval.value, PVPutType::Double));

    connect_pv(hlm, args.replace("$(P)$(M).HLM"), MonitorOn);
    hlm.set_component(PVInput(pvgroup.get_pv(hlm.pv_name), hlm.value, PVPutType::Double));
    
    connect_pv(llm, args.replace("$(P)$(M).LLM"), MonitorOn);
    llm.set_component(PVInput(pvgroup.get_pv(llm.pv_name), llm.value, PVPutType::Double));
    
    connect_pv(dhlm, args.replace("$(P)$(M).DHLM"), MonitorOn);
    dhlm.set_component(PVInput(pvgroup.get_pv(dhlm.pv_name), dhlm.value, PVPutType::Double));
    
    connect_pv(dllm, args.replace("$(P)$(M).DLLM"), MonitorOn);
    dllm.set_component(PVInput(pvgroup.get_pv(dllm.pv_name), dllm.value, PVPutType::Double));

    connect_pv(drbv, args.replace("$(P)$(M).DRBV"), MonitorOn);
}

MediumMotorDisplay::MediumMotorDisplay(PVGroup &pvgroup, const pvtui::ArgParser &args)
    : DisplayBase(pvgroup), args(args) {
    this->init();
}

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

    auto &desc_pv = pvgroup.get_pv(desc.pv_name);
    return ftxui::vbox({
	(desc_pv.connected() ? text("") : text("Disconnected") | color(Color::Red)) | center,

	desc.component()->Render() | color(Color::Black) | bgcolor(Color::RGB(210,210,210)) | size(WIDTH, EQUAL, 22) | center,
	separatorEmpty(),

	// 6 column hbox of vbox's
	// none | none | user | dial | lims/egu | spmg
	hbox({
	    filler() | size(WIDTH, EQUAL, egu.value.length()+8),
	    vbox({
		text("User") | center,
		hlm.component()->Render()
		    | center
		    | EPICSColor::EDIT
		    | size(WIDTH, EQUAL, 10),
		text(rbv.value)
		    | (use_set.value.index==0 ? EPICSColor::READBACK : color(Color::Yellow2))
		    | (dmov.value == 0 ? borderHeavy | color(Color::Green) : borderEmpty)
		    | center,
		val.component()->Render()
		    | center
		    | (able.value.index==0 ? EPICSColor::EDIT : ColorDisabled)
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
		text(drbv.value)
		    | (use_set.value.index==0 ? EPICSColor::READBACK : color(Color::Yellow2))
		    | (dmov.value == 0 ? borderHeavy | color(Color::Green) : borderEmpty)
		    | center,
		dval.component()->Render()
		    | center
		    | (able.value.index==0 ? EPICSColor::EDIT : ColorDisabled)
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
		text(hls.value ? unicode::rectangle(2) : "") | color(Color::Red),
		filler(),
		text(egu.value) | EPICSColor::READBACK,
		separatorEmpty(),
		text(lls.value ? unicode::rectangle(2) : "") | color(Color::Red),
	    }) | size(WIDTH, EQUAL, egu.value.length()),
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


void AllMotorDisplay::init() {

    using namespace pvtui;

    connect_pv(desc, args.replace("$(P)$(M).DESC"), MonitorOn);
    desc.set_component(PVInput(pvgroup.get_pv(desc.pv_name), desc.value, PVPutType::String));

    connect_pv(vmax, args.replace("$(P)$(M).VMAX"), MonitorOn);
    vmax.set_component(PVInput(pvgroup.get_pv(vmax.pv_name), vmax.value, PVPutType::Double));

    connect_pv(velo, args.replace("$(P)$(M).VELO"), MonitorOn);
    velo.set_component(PVInput(pvgroup.get_pv(velo.pv_name), velo.value, PVPutType::Double));
   
    connect_pv(vbas, args.replace("$(P)$(M).VBAS"), MonitorOn);
    vbas.set_component(PVInput(pvgroup.get_pv(vbas.pv_name), vbas.value, PVPutType::Double));
    
    connect_pv(accl, args.replace("$(P)$(M).ACCL"), MonitorOn);
    accl.set_component(PVInput(pvgroup.get_pv(accl.pv_name), accl.value, PVPutType::Double));
    
    connect_pv(mres, args.replace("$(P)$(M).MRES"), MonitorOn);
    mres.set_component(PVInput(pvgroup.get_pv(mres.pv_name), mres.value, PVPutType::Double));
    
    connect_pv(eres, args.replace("$(P)$(M).ERES"), MonitorOn);
    eres.set_component(PVInput(pvgroup.get_pv(eres.pv_name), eres.value, PVPutType::Double));

    connect_pv(rres, args.replace("$(P)$(M).RRES"), MonitorOn);
    rres.set_component(PVInput(pvgroup.get_pv(rres.pv_name), rres.value, PVPutType::Double));

    connect_pv(rtry, args.replace("$(P)$(M).RTRY"), MonitorOn);
    rtry.set_component(PVInput(pvgroup.get_pv(rtry.pv_name), rtry.value, PVPutType::Int));
    
    connect_pv(ueip, args.replace("$(P)$(M).UEIP"), MonitorOn);
    ueip.set_component(PVChoiceH(pvgroup.get_pv(ueip.pv_name), ueip.value.choices, ueip.value.index));
    
    connect_pv(urip, args.replace("$(P)$(M).URIP"), MonitorOn);
    urip.set_component(PVChoiceH(pvgroup.get_pv(urip.pv_name), urip.value.choices, urip.value.index));

    connect_pv(use_set, args.replace("$(P)$(M).SET"), MonitorOn);
    use_set.set_component(PVChoiceH(pvgroup.get_pv(use_set.pv_name), use_set.value.choices, use_set.value.index));
    
    connect_pv(cnen, args.replace("$(P)$(M).CNEN"), MonitorOn);
    cnen.set_component(PVChoiceH(pvgroup.get_pv(cnen.pv_name), cnen.value.choices, cnen.value.index));
    
    connect_pv(dir, args.replace("$(P)$(M).DIR"), MonitorOn);
    dir.set_component(PVChoiceH(pvgroup.get_pv(dir.pv_name), dir.value.choices, dir.value.index));

    connect_pv(able, args.replace("$(P)$(M)_able"), MonitorOn);
    able.set_component(PVChoiceV(pvgroup.get_pv(able.pv_name), able.value.choices, able.value.index));
    
    connect_pv(spmg, args.replace("$(P)$(M).SPMG"), MonitorOn);
    spmg.set_component(PVChoiceV(pvgroup.get_pv(spmg.pv_name), spmg.value.choices, spmg.value.index));
    
    connect_pv(foff, args.replace("$(P)$(M).FOFF"), MonitorOn);
    foff.set_component(PVDropdown(pvgroup.get_pv(foff.pv_name), foff.value.choices, foff.value.index));
    
    connect_pv(off, args.replace("$(P)$(M).OFF"), MonitorOn);
    off.set_component(PVInput(pvgroup.get_pv(off.pv_name), off.value, PVPutType::Double));

    connect_pv(prec, args.replace("$(P)$(M).PREC"), MonitorOn);
    prec.set_component(PVInput(pvgroup.get_pv(prec.pv_name), prec.value, PVPutType::Int));

    connect_pv(dmov, args.replace("$(P)$(M).DMOV"), MonitorOn);

    connect_pv(rbv, args.replace("$(P)$(M).RBV"), MonitorOn);
    
    connect_pv(drbv, args.replace("$(P)$(M).DRBV"), MonitorOn);
    
    connect_pv(val, args.replace("$(P)$(M).VAL"), MonitorOn);
    val.set_component(PVInput(pvgroup.get_pv(val.pv_name), val.value, PVPutType::Double));
    
    connect_pv(dval, args.replace("$(P)$(M).DVAL"), MonitorOn);
    dval.set_component(PVInput(pvgroup.get_pv(dval.pv_name), dval.value, PVPutType::Double));
    
    connect_pv(hlm, args.replace("$(P)$(M).HLM"), MonitorOn);
    hlm.set_component(PVInput(pvgroup.get_pv(hlm.pv_name), hlm.value, PVPutType::Double));
    
    connect_pv(llm, args.replace("$(P)$(M).LLM"), MonitorOn);
    llm.set_component(PVInput(pvgroup.get_pv(llm.pv_name), llm.value, PVPutType::Double));

    connect_pv(dhlm, args.replace("$(P)$(M).DHLM"), MonitorOn);
    dhlm.set_component(PVInput(pvgroup.get_pv(dhlm.pv_name), dhlm.value, PVPutType::Double));
    
    connect_pv(dllm, args.replace("$(P)$(M).DLLM"), MonitorOn);
    dllm.set_component(PVInput(pvgroup.get_pv(dllm.pv_name), dllm.value, PVPutType::Double));
   
    connect_pv(rval, args.replace("$(P)$(M).RVAL"), MonitorOn);
    rval.set_component(PVInput(pvgroup.get_pv(rval.pv_name), rval.value, PVPutType::Double));
    
    connect_pv(rrbv, args.replace("$(P)$(M).RRBV"), MonitorOn);

    connect_pv(twr, args.replace("$(P)$(M).TWR"), MonitorOff);
    twr.set_component(PVButton(pvgroup.get_pv(twr.pv_name), " < ", 1));

    connect_pv(twv, args.replace("$(P)$(M).TWV"), MonitorOn);
    twv.set_component(PVInput(pvgroup.get_pv(twv.pv_name), twv.value, PVPutType::Double));

    connect_pv(twf, args.replace("$(P)$(M).TWF"), MonitorOff);
    twf.set_component(PVButton(pvgroup.get_pv(twf.pv_name), " > ", 1));

    connect_pv(rlv, args.replace("$(P)$(M).RLV"), MonitorOn);
    rlv.set_component(PVInput(pvgroup.get_pv(rlv.pv_name), rlv.value, PVPutType::Double));

    connect_pv(egu, args.replace("$(P)$(M).EGU"), MonitorOn);
    egu.set_component(PVInput(pvgroup.get_pv(egu.pv_name), egu.value, PVPutType::String));
    
    connect_pv(hls, args.replace("$(P)$(M).HLS"), MonitorOn);

    connect_pv(lls, args.replace("$(P)$(M).LLS"), MonitorOn);
}

AllMotorDisplay::AllMotorDisplay(PVGroup &pvgroup, const pvtui::ArgParser &args)
    : DisplayBase(pvgroup), args(args) {
    this->init();
}

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
    auto &conn_pv = pvgroup.get_pv(desc.pv_name);

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
		text(rbv.value)
		    | (use_set.value.index==0 ? EPICSColor::READBACK : color(Color::Yellow2))
		    | center,
		val.component()->Render()
		    | center
		    | (able.value.index==0 ? EPICSColor::EDIT : ColorDisabled)
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
		text(drbv.value)
		    | (use_set.value.index==0 ? EPICSColor::READBACK : color(Color::Yellow2))
		    | center,
		dval.component()->Render()
		    | center
		    | (able.value.index==0 ? EPICSColor::EDIT : ColorDisabled)
		    | size(WIDTH, EQUAL, 10),
		dllm.component()->Render()
		    | EPICSColor::EDIT
		    | size(WIDTH, EQUAL, 10),
		filler() | size(WIDTH, EQUAL, 10),
	    }),
	    separatorEmpty(),
	    vbox({
		text("Raw") | center | color(Color::Black),
		hls.value == 1 ? text(unicode::rectangle(2)) | color(Color::Red) : text(""),
		text(rrbv.value) | EPICSColor::READBACK,
		rval.component()->Render() | EPICSColor::EDIT | size(WIDTH, EQUAL, 6),
		lls.value == 1 ? text(unicode::rectangle(2)) | color(Color::Red) : text(""),
	    }),
	    separatorEmpty(),
	    vbox({
		dmov.value==0 ? text("Moving")
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
