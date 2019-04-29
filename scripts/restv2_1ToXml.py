#!/usr/bin/python

# Convert an rml file from rest,v2.1 syntax into xml standard syntax

# Programme:
# 1) Remove the "section" keyword in front of "TRestXXX" tags.
# 2) Cancel the <environment> section and move its contents into the
#   <globals> section.
# 3) Avoid same attribute multiple times in a tag.
# 4) Close self closing tags with "/>" instead of ">"
# 5) Remove '<' and '>' from attribute values.
# 6) Leave "<for" opening tag open.
# 7) Add a root node.

# Caveats:
# 1) "section" is not removed if corresponding line is commented out.
# 2) The <environment> section is required to be unique,
#   and ahead of the <globals> section , if any.
# 3) Only the "units" attribute is considered. It is incorporated into
#   preceding numerics, whether "units" is specified multiple times or not.
#    If the value of the preceding attribute is a Linux environment variable,
#   e.g.: size="${REST_RADIUS}" units="mm", nothing is done, because I(Y.B.)
#   haven't yet found out whether one should proceed as for numerics above,
#   size="${REST_RADIUS}mm", or cancel the specification of "units" altogether
#   and ask the user to specify the unit in the variable. Which option is
#   going to be understood by REST_v2.2?
# 4) A restricted list is considered, viz.:
#   "<parameter", "<variable", "<observable", "canvas", "cut", ..., and "<addWHATEVER>".
#    A single tag per line and a single line par tag.
# 5) Only one occurence of each of '<' or '>' per line.
# 7) Root node is to be chosen among a built-in list.

# Linux XML tool "xmllint" is eventually used to check output file syntax.
# The script returns 0 if check is successful, returns 1 if not, and
# returns 2 when a processing error prevents it from being performed.

# NOTE:
# The list of self closing tags (see "selfCTags" infra) is limited in order to
# be on the safe side: unconstrained tag names may introduce mistakes that be
# difficult to indentify...

# STATUS:
# The script processes without error all rml files in PandXIII-REST, and
# REST_v2.1, but:
# - PandXIII-REST(b27d02f2)
# + "processes_(2|3)D.rml in "Topology-NEUTRINO2018", which have an unclosed
#  "TRestRawSignalAnalysisProcess" section.
# + "/readoutBarracon.rml" in "restG4sims/ZgzSimulations": it has an unclosed
#  "addReadoutModule" tag, several lines long, that the script cannot fix .
# + "microbulkModule.rml" in "readouts", which lacks an XML declaration.
# - REST_v2.1(c5171f61)
# + "isotopeFromVCylinder.rml" in "config/pandaXIII/templates": "units"
#  attributes referring to env. variables (see caveat (3)).
# + "gases.rml" in "data/definitions": comment all C++ ahead of section
#  tag.
# + "pixelReadout.rml" in "/data/definitions/readouts": for loop tag and
#  "<readoutModule" tag intertwined.
# + "readouts.rml" in "/data/definitions/readouts": many self closing
#  "<readoutModule" tags not properly closed (see caveat (4)).
# Nota Bena: The above means that output files passed succesfully the
# "xmllint" test and not that they are correctly understood by rest,v2.2.

from __future__ import print_function

import sys, os, argparse, re

##### SELF CLOSING TAGS considered
selfCTags = {"parameter","variable","observable","canvas","cut","energyDist","angularDist","style","labels","legendPosition"} # + addWHATEVER

def printMoreHelp():
    print('restv2_1ToXml: Convert an rml file from rest,v2.1 syntax into xml standard syntax\n')

    print('Programme:')
    print(' 1) Remove the "section" keyword in front of "TRestXXX" tags.')
    print(' 2) Cancel the <environment> section and move its contents into the')
    print('   <globals> section.')
    print(' 3) Avoid same attribute multiple times in a tag.')
    print(' 4) Close self closing tags with "/>" instead of ">".')
    print(" 5) Remove '<' and '>' from attribute values.")
    print(' 6) Leave "<for" opening tag open.')
    print(' 7) Add a root node.')

    print('\nCaveats:')
    print(' 1) "section" is not removed if corresponding line is commented out.')
    print(' 2) The <environment> section is required to be unique,')
    print('   and ahead of the <globals> section, if any.')
    print(' 3) Only the "units" attribute is considered. It is incorporated into')
    print('   preceding numerics, whether "units" is specified multiple times or not.')
    print('    If the value of the preceding attribute is a Linux environment variable,')
    print('   e.g.: size="${REST_RADIUS}" units="mm", nothing is done, because I(Y.B.)')
    print("   haven't yet found out whether one should proceed as for numerics above,")
    print('   size="${REST_RADIUS}mm", or cancel the specification of "units" altogether')
    print('   and ask the user to specify the unit in the variable. Which option is')
    print('   going to be understood by REST_v2.2?')
    print(' 4) A restricted list is considered, viz.:\n '),
    for selfCTag in selfCTags:
        print(' "%s"' % selfCTag),
    print(' and "addWHATEVER"')
    print('    A single tag per line and a single line par tag.')
    print(" 5) Only one occurence of each of '<' or '>' per line.")
    print(' 7) Root node is to be chosen among a built-in list')

    print('\nLinux XML tool "xmllint" is eventually used to check output file syntax.')
    print('The script returns 0 if check is successful, returns 1 if not, and')
    print('returns 2 when a processing error prevents it from being performed.')

def main():

    ##### PARSE COMMAND LINE
    # Built-in list of possible choices for rootNode name:
    rootNodes = {'restG4','restManager','processes','gases','readouts'}
    # Option MORE HELP
    if len(sys.argv) > 1 and (sys.argv[1] == '-H' or sys.argv[1] == '-HELP'):
        printMoreHelp()
        sys.exit(2)
    # "argparse" PARSER
    parser = argparse.ArgumentParser(description='Convert an rml file from rest,v1 syntax into xml standard syntax')
    parser.add_argument("rmlInFile",help="Input file name")
    parser.add_argument("rmlOutFile",help="Output file name")
    parser.add_argument('rootNode',choices=rootNodes,help="Tag name assigned to root node")
    parser.add_argument('-H','--HELP',dest='moreHelp',action='store_true',help="More help")
    parser.add_argument('-i','--indent',dest='doIndent',action='store_true',help='Indent (note: needs "emacs")')
    parser.add_argument('-q','--quiet',dest='quiet',action='store_true',help='Less verbosity')
    args = parser.parse_args()
    verbose = args.quiet == False

    ##### INPUT FILE
    # Open
    rmlInF = "local"
    try:
        rmlInF = open(args.rmlInFile,"r")
    except IOError, e:
        sys.stderr.write('%s\n** restv2_1ToXml: Error opening "%s" input <rmlInFile> => Exiting...\n' % (str(e),args.rmlInFile)
        sys.exit(2)
    # Copy contents to array "lines"
    lines = []
    for line in rmlInF:
        lines.append(line)
    rmlInF.close()

    ##### OUTPUT FILE: Already exists?
    if os.path.exists(args.rmlOutFile):
        sys.stderr.write('** restv2_1ToXml: <rmlOutFile> "%s" already exists => Please remove...\n' % args.rmlOutFile)
        sys.exit(2)


    ###########   LOOP ON LINES IN INPUT FILE   ##########
    print(' * restv2_1ToXml: Processing input <rmlInFile> "%s"' % args.rmlInFile))
    newlines = []
    newline = ""
    count = newcount = -1
    declaration = False
    comment = environment = globalsTag = rootNode = section = sections = 0
    envlines = []
    skip = True
    sectionName = ""
    actions = unitsFixes = selfCFixes = attValFixes = forLoopFixes = 0
    rootNodeMatch = "^\s*<%s>" % args.rootNode
    for line in lines:
        #print("++ %d %d %d %d %d %d" % (declaration,comment,environment,globalsTag,len(envlines),len(newlines))) # for debugging...
        count += 1
        if skip == False:
            newlines.append(newline)
            newcount += 1
        skip = False
        newline = line

        ##### XML DECLARATION: "must be situated at the first position of the first line"
        # Opening
        if count == 0:
            # (Note: we assume here that XML declaration is mandatory.)
            m = re.match("^<\?xml",line)
            if m:
                m = re.match("\?>",line)
                if m == 0:
                    declaration = True
                continue
            else:
                sys.stderr.write("\n** restv2_1ToXml: Input <rmlInFile> does not start with an XML declaration (\"<?xml\") => Exiting...\n")
                sys.exit(2)
        # Closing
        if declaration:
            m = re.match("?>$",line)
            if m:
                declaration = False
            continue

        ##### BLANK LINE: Skip, so that rootNode be inserted just ahead of
        # first meaningful, not-comment-or-blank, line
        m = re.match(r'\s*$',line)
        if m and environment <= 0: # Keep
            continue

        ##### COMMENT:
        # - Skip header comment, so that rootNode be inserted afterward
        # - Do not modify any comment in any way (note that this may not be the
        #  most appropriate: one would want a commented out "<section TRestXXX"
        #  string to be also modified and stripped of its leading "section",
        #  but too bad...)
        # Opening
        if comment == 0 and environment <= 0: # Comments in <environment> are to be later saved in "envlines" array (and left unchanged)
            m = re.match(r'\s*<!--',line)
            if m:
                comment = count # It cannot be =0, since 0th is the declaration line
        # Closing
        if comment != 0:
            m = re.search(r'-->\s*$',line)
            if m:
                comment = 0
            continue

        ##### ROOT NODE
        if rootNode == 0:
            rootNode = count
        # PRE-EXISTING ROOT NODE?
        # (This check to identify cases where an already converted rml file
        # is submitted by mistake.)
        m = re.match(rootNodeMatch,line)
        if m:
            sys.stderr.write('\n** restv2_1ToXml: Input <rmlInFile> contains tag "<%s>" which happends to bear the name of arg. <rootNode> => Most probably something got messed up on the command line => Please double-check your inputs...\n' % args.rootNode)
            sys.exit(2)

        ##### ENVIRONMENT
        # Opening
        m = re.match("\s*<environment>\s*$",line)
        if environment == 0:
            if m:
                environment = count # It cannot be =0, since 0th is the declaration line
                skip = True # So as to not copy "<environment>" opening
                continue
        # Closing
        elif environment > 0:
            if m:
                sys.stderr.write("** restv2_1ToXml: Input <rmlInFile> found to open a new <environment> tag (at line %d) while previous such tag, opened at line %d, is not closed => Exiting...\n" % (count+1,environment+1))
                sys.exit(2)
        m = re.match("\s*</environment>\s*$",line)
        if m:
            if environment > 0:
                environment *= -1 # Flag the fact that it's closed, but also remember the line#
                skip = True # So as to not copy "</environment>" closing
                continue
            else:
                sys.stderr.write("** restv2_1ToXml: Input <rmlInFile> has </environment> closing tag (at line %d) while it's found to have not opened any <environment> section  => Exiting...\n" % count+1)
                sys.exit(2)
        # Body
        # - Save <environment> lines to "envlines" array.
        if environment > 0:
            envlines.append(line)
            skip = True

        ##### GLOBALS
        # Opening: <globals>:
        # - Insert <environment> lines,
        # - preceded by an introductory comment,
        # - followed by a comment introducing proper <globals> lines .
        if globalsTag == 0:
            m = re.match(r'\s*<globals>\s*$',line)
            if m:
                if verbose:
                    print("  Move <environment> section into <globals>")
                globalsTag = newcount + 1
                sections += 1
                actions |= 2

        ##### SECTION
        isSectionTag = 0
        m = re.match("\s*<section\s+\S",line)
        if section == 0:
            if m:
                isSectionTag = 1
                newline = re.sub(r'^(\s*<)section\s+(\S)',r'\1\2',line)
                line = newline
                sectionName = re.sub(r'^\s*<(\S+)\s.*\n$',r'\1',newline)
                section = count
                # Opening and closing may happen to be on same line
                # Then edit the closing tag right away.
                m = re.search("\s*</section>\s*",line)
                if m:
                    if verbose:
                        print('  Fix tag %s' % sectionName)
                    repl = "\\1</%s>" % sectionName
                    newline = re.sub(r'(\s*)</section>',repl,newline)
                    line = newline
                    sections += 1
                    actions |= 1
                    section = 0
        else:
            if m:
                line1 = re.sub(r'(.*)\n$',r'\1',lines[section])
                line2 = re.sub(r'(.*)\n$',r'\1',line)
                sys.stderr.write('\n** restv2_1ToXml: Input <rmlInFile> found to open a new <section at line %d:\n  "%s"\nwhile previous <section, opened at line %d:\n  "%s"\nis not closed => Exiting...\n' % (count+1,line2,section+1,line1))
                sys.exit(2)
            m = re.match("\s*</section>\s*",line)
            if m:
                if verbose:
                    print('  Fix tag %s' % sectionName)
                newline = "</%s>\n" % sectionName
                sections += 1
                actions |= 1
                section = 0

        ##### MULTIPLE ATTRIBUTE:
        # - Only "units" is considered
        # - Ocurrences of "attribute=numerics units=string" are converted into
        #  "attribute=numericsstring", whether or not multiple "units" are
        #  present.
        m = re.search(r'units="[A-Za-z]+"',line)
        if m:
            # Python has a special sequence "\w" for matching alphanumeric and underscore when the LOCALE and UNICODE flags are not specified.
            newline = re.sub(r'(\s\w+="[\+\-0-9\.(,)]+)" units="([A-Za-z]+)"',r'\1\2"',line)
            line = newline
            unitsFixes += 1
            actions |= 4

        ##### ATRIBUTE VALUES WITH '<' or '>'
        m = re.match(r'^\s*<[^>]*<',line)
        if m:
            newline = re.sub(r'^(\s*<[^>]*)<',r'\1&lt;',line)
            line = newline
            attValFixes += 1
            actions |= 16
        m = re.search(r'>[^<]*>\s*$',line)
        if m:
            newline = re.sub(r'>([^<]*>)\s*$',r'&gt;\1\n',line)
            line = newline
            attValFixes += 1
            actions |= 16

        ##### FOR LOOP
        m = re.match(r'^\s*<for .*/>\s*$',line)
        if m:
            newline = re.sub(r'^(\s*<for .*)/>\s*$',r'\1>\n',line)
            line = newline
            forLoopFixes += 1
            actions |= 32

        ##### SELF CLOSING TAG
        badSelfCTag = 0
        # Section tag is not self closing, yet may fulfill the criteria below,
        # if its name starts, e.g., w/ "add".
        if isSectionTag == 0:
            m = re.match(r'^\s*<(\w+)\s+[^>]+[^/]>',line)
            if m:
                tag = m.group(1)
                for selfCTag in selfCTags:
                    if tag == selfCTag:
                        badSelfCTag = 1
                        break
            if badSelfCTag == 0:
                m = re.match(r'\s*<add',line) # <addWHATEVER
                if m:
                    badSelfCTag = 1
        if badSelfCTag:
            # Firstly, handle the case where self closing tag is followed by a comment
            m = re.match(r'^\s*<[^>]+[^/]>\s*<!--.+-->\s*$',line)
            if m:
                newline = re.sub(r'>(\s*<!--.+-->)\s*$',r'/>\1\n',line)
                selfCFixes += 1
                actions |= 8
            else:
                m = re.match(r'^\s*<[^>]+[^/]>\s*$',line)
                if m:
                    newline = re.sub(r'>\s*$',"/>\n",line)
                    selfCFixes += 1
                    actions |= 8
                else: # Include also cases where a comment introduced w/ '%' is appended
                    m = re.match(r'^\s*<[^>]+[^/]>\s*%.*$',line)
                    if m:
                        newline = re.sub(r'>(\s*%.*)$',r'/>\1\n',line)
                        selfCFixes += 1
                        actions |= 8
                    else: # Include also cases where a comment introduced w/ "//" is appended
                        m = re.search(r'^\s*<[^>]+[^/]>\s*//.*$',line)
                        if m:
                            newline = re.sub(r'>(\s*//.*)$',r'/>\1\n',line)
                            selfCFixes += 1
                            actions |= 8

    ##### LAST LINE
    if skip == False:
        newlines.append(newline)

    ##### More verbosity...
    if verbose:
        if unitsFixes:
            print('  %d occurrences of "units" attribute processed' % unitsFixes)
        if selfCFixes:
            print('  %d self closing tags processed' % selfCFixes)
        if attValFixes:
            print("  %d attribute values with '<' or '>' processed" % attValFixes)
        if forLoopFixes:
            print("  %d for loops processed" % forLoopFixes)

    ##### PENDING declaration, comment, environment => ERROR SYNTAX
    if declaration:
        sys.stderr.write('** restv2_1ToXml: Input <rmlInFile> opens an XML declaration ("<?xml") but never close it ("?>") => Exiting...\n')
        sys.exit(2)
    if comment != 0:
        line = re.sub(r'(.*)\n$',r'\1',lines[comment])
        sys.stderr.write('** restv2_1ToXml: Input <rmlInFile> found to never close comment opened at line %d:\n  "%s"\n   => Exiting...\n' % (comment+1,line))
        sys.exit(2)
    if environment > 0:  # Should end up being <0. Does not if not closed by </environment>
        sys.stderr.write("** restv2_1ToXml: Input <rmlInFile> found to never close <environment> tag opened at line %d  => Exiting...\n" % (environment+1))
        sys.exit(2)
    if section:
        line = re.sub(r'(.*)\n$',r'\1',lines[section])
        sys.stderr.write('** restv2_1ToXml: Input <rmlInFile> found to never close <section opened at line %d:\n "%s"\n   => Exiting...\n' % (section+1,line))
        sys.exit(2)

    ##### NO ACTION TAKEN => EXIT
    if actions == 0:
        print('  No modification required => Exiting without creating <rmlOutFile> "%s"' % args.rmlOutFile)
        sys.exit(0)

    ##### OPEN OUTPUT FILE
    print('\n * restv2_1ToXml: Writing <rmlOutFile> "%s"' % args.rmlOutFile)
    rmlOutF = "local"
    try:
        rmlOutF = open(args.rmlOutFile,"w")
    except IOError, e:
        sys.stderr.write('%s\n** restv2_1ToXml: Error opening "%s" out <rmlOutFile> => Exiting...\n' % (str(e),args.rmlOutFile))
        sys.exit(2)

    # WRITE OUTPUT FILE
    # Write "newlines", w/ some finalisation processing
    environment *= -1
    if environment > 0 and globalsTag == 0:
        sections += 1
    count = -1
    for newline in newlines:
        count += 1
        if sections > 1 and count == rootNode:
            if verbose:
                print('  Add root node tag "%s"' % args.rootNode)
            rmlOutF.write("<%s>\n" % args.rootNode)
            if environment > 0 and globalsTag == 0:
                if verbose:
                    print('  Add <globals> tag')
                rmlOutF.write("\n<globals>\n")
                for envline in envlines:
                    rmlOutF.write(envline)
                rmlOutF.write("</globals>\n")
            if globalsTag:
                globalsTag += 1
        if environment > 0 and globalsTag and count == globalsTag:
            rmlOutF.write("	<!-- Environment -->\n")
            for envline in envlines:
                rmlOutF.write(envline)
            rmlOutF.write("	<!-- Globals proper -->\n")
        #http://legacy.python.org/search/hypermail/python-1992/0115.html
        #Is it possible to print(something but not automatically have a carriage return appended to it ?
        #Yes, append a comma after the last argument to print.
        #print(newline),
        rmlOutF.write("%s" % newline)
    if sections > 1:
        rmlOutF.write("</%s>\n" % args.rootNode)
    rmlOutF.close()

    ##### CHECK RESULTING FILE
    print('\n * restv2_1ToXml: Check syntax with "xmllint":')
    command = "xmllint %s > /dev/null" % args.rmlOutFile
    status = os.system(command)
    if status:
        print('\n  => "xmllint" returns in error => Please double-check input file')
        sys.exit(1)
    else:
        print("   => OK!")

    ##### INDENT
    # It's only done if the "xmllint" check goes well, for else output file
    # may have to be investigated visually and compared to input fiel: better
    # then that we limit the aount of differences to the minimum...
    if args.doIndent:
        status = os.system("which emacs >& /dev/null")
        if status:
            print('\n** restv2_1ToXml: No indenting output <rmlOutFile>: "emacs" not available')
        else:
            print('\n * restv2_1ToXml: Indentation with "emacs"')
            command = "emacs -Q -batch %s --eval '(indent-region (point-min) (point-max) nil)' -f save-buffer > /dev/null" % args.rmlOutFile
            status = os.system(command)

if __name__ == "__main__":
  main()
