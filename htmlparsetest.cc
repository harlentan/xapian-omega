/* htmlparsetest.cc: test the MyHtmlParser class
 *
 * Copyright (C) 2006,2008,2011,2012,2013 Olly Betts
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301
 * USA
 */

#include <config.h>

#include <cstdlib>
#include <iostream>
#include <string>

#include "myhtmlparse.h"

using namespace std;

struct testcase {
    const char * html;
    const char * dump;
    const char * title;
    const char * keywords;
    const char * sample;
};

static const testcase tests[] = {
    { "<body>test<!--htdig_noindex-->icle<!--/htdig_noindex-->s</body>",
      "tests", "", "", "" },
    { "<body>test<!--htdig_noindex-->ing</body>", "test", "", "", "" },
    { "hello<!-- bl>ah --> world", "hello world", "", "", "" },
    { "hello<!-- blah > world", "hello world", "", "", "" },
    { "<script>\nif (a<b) a = b;</script>test", "test", "", "", "" },
    // Regression test for bug first noticed in 1.0.0 (but present earlier).
    { "<b>not</b>\n<b>able</b>", "not able", "", "", "" },
    // Check that whitespace is handled as intended.
    { " <b>not </b>\n<b>\table\t</b>\r\n", "not able", "", "", "" },
    { "<html><head><title>\xc2\xae</title></head><body>\xc2\xa3</body></html>", "\xc3\x82\xc2\xa3", "\xc3\x82\xc2\xae", "", "" },
    { "<html><head><meta http-equiv=Content-Type content=\"text/html;charset=iso-8859-1\"><title>\xc2\xae</title></head><body>\xc2\xa3</body></html>", "\xc3\x82\xc2\xa3", "\xc3\x82\xc2\xae", "", "" },
    { "<html><head><meta http-equiv=Content-Type content=\"text/html;charset=utf-8\"><title>\xc2\xae</title></head><body>\xc2\xa3</body></html>", "\xc2\xa3", "\xc2\xae", "", "" },
    { "<html><head><meta charset='utf-8'><title>\xc2\xae</title></head><body>\xc2\xa3</body></html>", "\xc2\xa3", "\xc2\xae", "", "" },
    { "<html><head><title>\xc2\xae</title><meta charset=\"utf-8\"></head><body>\xc2\xa3</body></html>", "\xc2\xa3", "\xc2\xae", "", "" },
    { "<!--UdmComment-->test<!--/UdmComment--><div id='body'>test</div>", "test", "", "", "" },
    { "Foo<![CDATA[ & bar <literal>\"]]> ok", "Foo & bar <literal>\" ok", "", "", "" },
    { "Foo<![CDATA", "Foo", "", "", "" },
    { "foo<![CDATA[bar", "foobar", "", "", "" },
    // Test that handling of multiple body tags matches modern browser behaviour (ticket#599).
    { "a<html>b<head>c<title>bad</title>d</head>e<body>f</body>g<body>h</body>i</html>j<body>k", "abcdefghijk", "bad", "", "" },
    { "check<object id='foo'>for<applet foo=\"bar\" />spaces<br> in <p>\tout</p>put\r\n", "check for spaces\rin\rout\rput", "", "", "" },
    { "tab:<table><tr><th>col 1</th><th>col 2</th></tr><tr><td>test</td><td><img src='foo.jpg'> <img src='bar.jpg'></td></tr><tr><td colspan=2>hello world</td></tr></table>done", "tab:\rcol 1\tcol 2\rtest\rhello world\rdone", "", "", "" },
    { 0, 0, 0, 0, 0 }
};

int
main()
{
    for (size_t i = 0; tests[i].html; ++i) {
	MyHtmlParser p;
	try {
	    p.parse_html(tests[i].html, "iso-8859-1", false);
	} catch (const string &newcharset) {
	    p.reset();
	    p.parse_html(tests[i].html, newcharset, true);
	}
	if (!p.indexing_allowed) {
	    cout << "indexing disallowed by meta tag - skipping\n";
	    continue;
	}
	if (tests[i].dump != p.dump) {
	    cout << "DUMP " << i << ": [" << p.dump << "] != [" << tests[i].dump << "]" << endl;
	    exit(1);
	}
	if (tests[i].title != p.title) {
	    cout << "TITLE " << i << ": [" << p.title << "] != [" << tests[i].title << "]" << endl;
	    exit(1);
	}
	if (tests[i].keywords != p.keywords) {
	    cout << "KEYWORDS " << i << ": [" << p.keywords << "] != [" << tests[i].keywords << "]" << endl;
	    exit(1);
	}
	const char *sample = tests[i].sample;
	if (sample == NULL) sample = tests[i].dump;
	if (sample != p.sample) {
	    cout << "SAMPLE " << i << ": [" << p.sample << "] != [" << sample << "]" << endl;
	    exit(1);
	}
    }
}
