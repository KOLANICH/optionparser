/* Written 2012 by Matthias S. Benkmann
 *
 * The author hereby waives all copyright and related rights to the contents
 * of this example file (example_arg.cc) to the extent possible under the law.
 */

/**
 * @file
 * @brief Demonstrates handling various types of option arguments (required, numeric,...).
 *
 * @include example_arg.cc
 */

#include <stdio.h>
#include <stdlib.h>
#include "optionparser.h"

struct Arg: public option::Arg
{
  static option::ArgStatus Unknown(const option::Option& option, bool msg)
  {
    if (msg) fprintf(stderr, "Unknown option '%s'\n", option.name);
    return option::ARG_ILLEGAL;
  }

  static option::ArgStatus Required(const option::Option& option, bool msg)
  {
    if (option.arg != 0)
      return option::ARG_OK;

    if (msg) fprintf(stderr, "Option '%s' requires an argument\n", option.name);
    return option::ARG_ILLEGAL;
  }

  static option::ArgStatus NonEmpty(const option::Option& option, bool msg)
  {
    if (option.arg != 0 && option.arg[0] != 0)
      return option::ARG_OK;

    if (msg) fprintf(stderr, "Option '%s' requires a non-empty argument\n", option.name);
    return option::ARG_ILLEGAL;
  }

  static option::ArgStatus Numeric(const option::Option& option, bool msg)
  {
    char* endptr = 0;
    if (option.arg != 0 && strtol(option.arg, &endptr, 10)){};
    if (endptr != option.arg && *endptr == 0)
      return option::ARG_OK;

    if (msg) fprintf(stderr, "Option '%s' requires a numeric argument\n", option.name);
    return option::ARG_ILLEGAL;
  }
};

enum  optionIndex { UNKNOWN, HELP, OPTIONAL, REQUIRED, NUMERIC, NONEMPTY };
const option::Descriptor usage[] = {
{ UNKNOWN, 0,"", "",        Arg::Unknown, "USAGE: example_arg [options]\n\n"
                                          "Options:" },
{ HELP,    0,"", "help",    Arg::None,    "  \t--help  \tPrint usage and exit." },
{ OPTIONAL,0,"o","optional",Arg::Optional,"  -o [<arg>], \t--optional[=<arg>]"
                                          "  \tTakes an argument but is happy without one." },
{ REQUIRED,0,"r","required",Arg::Required,"  -r <arg>, \t--required=<arg>  \tMust have an argument." },
{ NUMERIC, 0,"n","numeric", Arg::Numeric, "  -n <num>, \t--numeric=<num>  \tRequires a number as argument." },
{ NONEMPTY,0,"1","nonempty",Arg::NonEmpty,"  -1 <arg>, \t--nonempty=<arg>"
                                          "  \tCan NOT take the empty string as argument." },
{ UNKNOWN, 0,"", "",        Arg::None,
 "\nExamples:\n"
 "  example_arg --unknown -o -n10 \n"
 "  example_arg -o -n10 file1 file2 \n"
 "  example_arg -nfoo file1 file2 \n"
 "  example_arg --optional -- file1 file2 \n"
 "  example_arg -o file1 file2 \n"
 "  example_arg -unk file1 file2 \n"
 "  example_arg -r -- file1 \n"
 "  example_arg --required \n"
 "  example_arg --nonempty= file1 \n"
 "  example_arg --nonempty=foo --numeric=999 --optional=bla file1 \n"
},
{ 0, 0, 0, 0, 0, 0 } };

int main(int argc, char* argv[])
{
  argc-=(argc>0); argv+=(argc>0); // skip program name argv[0] if present
  option::Stats stats(usage, argc, argv);
  option::Option options[stats.options_max], buffer[stats.buffer_max];
  option::Parser parse(usage, argc, argv, options, buffer);

  if (parse.error())
    return 1;

  if (options[HELP] || argc == 0)
  {
    int columns = getenv("COLUMNS")? atoi(getenv("COLUMNS")) : 80;
    option::printUsage(fwrite, stdout, usage, columns);
    return 0;
  }

  for (int i = 0; i < parse.optionsCount(); ++i)
  {
    option::Option& opt = buffer[i];
    fprintf(stdout, "Argument #%d is ", i);
    switch (opt.index())
    {
      case HELP:
        // not possible, because handled further above and exits the program
      case OPTIONAL:
        if (opt.arg)
          fprintf(stdout, "--optional with optional argument '%s'\n", opt.arg);
        else
          fprintf(stdout, "--optional without the optional argument\n");
        break;
      case REQUIRED:
        fprintf(stdout, "--required with argument '%s'\n", opt.arg);
        break;
      case NUMERIC:
        fprintf(stdout, "--numeric with argument '%s'\n", opt.arg);
        break;
      case NONEMPTY:
        fprintf(stdout, "--nonempty with argument '%s'\n", opt.arg);
        break;
      case UNKNOWN:
        // not possible because Arg::Unknown returns ARG_ILLEGAL
        // which aborts the parse with an error
        break;
    }
  }

  for (int i = 0; i < parse.nonOptionsCount(); ++i)
    fprintf(stdout, "Non-option argument #%d is %s\n", i, parse.nonOption(i));
}
