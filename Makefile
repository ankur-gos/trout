# $Id: Makefile,v 1.23 2016-10-12 16:59:53-07 - - $

DEPSFILE  = Makefile.deps
NOINCLUDE = ci clean spotless
NEEDINCL  = ${filter ${NOINCLUDE}, ${MAKECMDGOALS}}
CPP       = g++ -g -O0 -Wall -Wextra -std=gnu++14
MKDEPS    = g++ -MM -std=gnu++14
GRIND     = valgrind --leak-check=full --show-reachable=yes
FLEX      = flex --outfile=${LEXCPP}
BISON     = bison --defines=${PARSEHDR} --output=${PARSECPP} --xml

MODULES   = astree lyutils stringset auxlib
HDRSRC    = ${MODULES:=.h}
CPPSRC    = ${MODULES:=.cpp} oc.cpp
FLEXSRC   = scanner.l
BISONSRC  = parser.y
PARSEHDR  = yyparse.h
LEXCPP    = yylex.cpp
PARSECPP  = yyparse.cpp
CGENS     = ${LEXCPP} ${PARSECPP}
ALLGENS   = ${PARSEHDR} ${CGENS}
EXECBIN   = oc
ALLCSRC   = ${CPPSRC} ${CGENS}
OBJECTS   = ${ALLCSRC:.cpp=.o}
LEXOUT    = yylex.output
PARSEOUT  = yyparse.output
REPORTS   = ${LEXOUT} ${PARSEOUT}
MODSRC    = ${foreach MOD, ${MODULES}, ${MOD}.h ${MOD}.cpp}
MISCSRC   = ${filter-out ${MODSRC}, ${HDRSRC} ${CPPSRC}}
ALLSRC    = README ${FLEXSRC} ${BISONSRC} ${MODSRC} ${MISCSRC} Makefile

all : ${EXECBIN}

${EXECBIN} : ${OBJECTS}
	${CPP} -o${EXECBIN} ${OBJECTS}

yylex.o : yylex.cpp
	# Suppress warning message from flex compilation.
	${CPP} -Wno-sign-compare -c $<

%.o : %.cpp
	${CPP} -c $<

${LEXCPP} : ${FLEXSRC}
	${FLEX} ${FLEXSRC}

${PARSECPP} ${PARSEHDR} : ${BISONSRC}
	${BISON} ${BISONSRC}

clean :
	- rm ${OBJECTS} ${ALLGENS} ${REPORTS} ${DEPSFILE}

spotless : clean
	- rm ${EXECBIN}

deps : ${ALLCSRC}
	@ echo "# ${DEPSFILE} created `date` by ${MAKE}" >${DEPSFILE}
	${MKDEPS} ${ALLCSRC} >>${DEPSFILE}

${DEPSFILE} :
	@ touch ${DEPSFILE}
	${MAKE} --no-print-directory deps
	
ifeq "${NEEDINCL}" ""
include ${DEPSFILE}
endif

