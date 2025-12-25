#include <iostream>
#include <pvtui/pvtui.hpp>


int main() {

    std::cout << "[pvtui::ArgParser] Running tests...\n";

    {
	char arg0[] = "ArgParser test";
	char arg1[] = "--macro";
	char arg2[] = "P=MyPrefix:";
	char *args[] = {arg0, arg1, arg2, nullptr};
	pvtui::ArgParser parser(3, args);

	assert(parser.macros_present({"P"}));
	assert(parser.replace("$(P)") == "MyPrefix:");
    }

    {
	char arg0[] = "ArgParser test";
	char arg1[] = "--macro";
	char arg2[] = "P=MyPrefix:,M=m1";
	char *args[] = {arg0, arg1, arg2, nullptr};
	pvtui::ArgParser parser(3, args);

	assert(parser.macros_present({"P"}));
	assert(parser.macros_present({"M"}));
	assert(parser.macros_present({"P", "M"}));
	assert(parser.replace("$(P)") == "MyPrefix:");
	assert(parser.replace("$(M)") == "m1");
    }

    {
	char arg0[] = "ArgParser test";
	char arg1[] = "--macro";
	char arg2[] = "P=MyPrefix:,MOTOR=m1";
	char *args[] = {arg0, arg1, arg2, nullptr};
	pvtui::ArgParser parser(3, args);

	assert(parser.macros_present({"P"}));
	assert(parser.macros_present({"MOTOR"}));
	assert(parser.macros_present({"P", "MOTOR"}));
	assert(parser.replace("$(P)") == "MyPrefix:");
	assert(parser.replace("$(MOTOR)") == "m1");
    }

    {
	char arg0[] = "ArgParser test";
	char arg1[] = "--macro";
	char arg2[] = "PREF=MyPrefix:,MOTOR=m1";
	char *args[] = {arg0, arg1, arg2, nullptr};
	pvtui::ArgParser parser(3, args);

	assert(parser.macros_present({"MOTOR"}));
	assert(parser.macros_present({"PREF"}));
	assert(parser.macros_present({"MOTOR", "PREF"}));
	assert(parser.replace("$(PREF)") == "MyPrefix:");
	assert(parser.replace("$(MOTOR)") == "m1");
    }

    std::cout << "[pvtui::ArgParser] All tests passed" << std::endl;

}
