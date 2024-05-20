# vim: ts=4 sw=4 noet:
#==================================================================================
#    Copyright (c) 2020 AT&T Intellectual Property.
#    Copyright (c) 2020 Nokia
#
#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.
#==================================================================================

# filename prefix to add when publishing docs to the scrapped directory.
#
doc_prefix = mgxapp_

# overrride from command line if {X}fm is not installed in the standard place
# the rules will _always_ add the current directory first, so this just needs
# to reference the location of the {X}fm imbed (.im) files installed with the
# {X}fm package
#
XPATH = /usr/local/share/xfm:..

%.txt: %.xfm
	PASS=1 XFM_OUTPUT_TYPE=txt TFM_PATH=.:$(XPATH) tfm $< /dev/null
	PASS=2 XFM_OUTPUT_TYPE=txt TFM_PATH=.:$(XPATH) tfm $< $@

# md and rst are space sensitive parsers and the leading blank that tfm inserts must
# be stripped. Trailing whitespace IS important for md, so do NOT strip that.
#
%.md: %.xfm
	PASS=1 XFM_OUTPUT_TYPE=md TFM_PATH=.:$(XPATH) tfm $< /dev/null
	PASS=2 XFM_OUTPUT_TYPE=md TFM_PATH=.:$(XPATH) tfm $< stdout | sed 's!^ !!' >$@

%.rst: %.xfm
	PASS=1 XFM_OUTPUT_TYPE=rst TFM_PATH=.:$(XPATH) tfm $< /dev/null
	PASS=2 XFM_OUTPUT_TYPE=rst TFM_PATH=.:$(XPATH) tfm $< stdout | sed 's!^ !!; s! *$$!!' >$@

%.ps: %.xfm
	IN_FILE=$@ \
	&& IN_FILE=$${IN_FILE%.*} XFM_OUTPUT_TYPE=ps PASS=1 XFM_PATH=.:$(XPATH) pfm $< /dev/null \
	&& IN_FILE=$${IN_FILE%.*} XFM_OUTPUT_TYPE=ps PASS=2 XFM_PATH=.:$(XPATH) pfm $< $@

# requires ghostscript to be installed
%.pdf: %.ps
	gs -dBATCH  -dNOPROMPT -dNOPAUSE -sDEVICE=pdfwrite -sOutputFile=$@ $<


# ------------- generic all rule ----------------------------------------------------

# ALL_LIST names each doc; we'll generate all forms of supported output
all: $(ALL_LIST:%=%.ps) $(ALL_LIST:%=%.md) $(ALL_LIST:%=%.txt) $(ALL_LIST:%=%.rst)

# ----------- publishing generatd output --------------------------------------------

# copy the .rst versions to the top level docs directory for RTD scraping
# we must prefix documents because the scraper is dumb and doesn't support
# layers in the docs directory. We assume that this this directory is under
#   repo/project/docs/src
# and thus the pulication directory is repo/docs with the relative path
# to copy files up from a subdirectory would be ../../../../docs/
#
#
publish: $(ALL_LIST:%=%.rst)
	for f in *.rst; \
	do \
		cp $$f ../../../../docs/$(doc_prefix)$$f; \
	done


# ----- housekeeping ------------------------------------------------------------------

# remove any intermediate file; leave final output files
clean:
	rm -f *.bcnfile *.ca *.sp *.toc

# trash anything that can be built
nuke: clean
	rm -f *.pdf *.ps *.rst *.md *.txt
