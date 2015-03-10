// Generate some autodoc pages by running
// ../hcana makehtmldoc.C
makehtmldoc() {
 THtml html;
 // Enable the "viewVC header" and "viewVC source" links to work
 // But will only work for hcana classes.  Links will be broken for
 // THa classes.
 // Could probably go through all the THa*.html files that are created
 // and fix these links, but would need to know the podd branch we are on.
 html.SetViewCVS("https://github.com/JeffersonLab/hcana/tree/develop/");
 html.SetProductName("Hall C Analysis Code HCANA");
 html.SetSourceDir("..:../podd");
 html.SetIncludePath("..");
 html.SetHeader("doc_header.html");
 html.SetFooter("doc_footer.html");
 // Create documentation pages for all Classes, hcana and podd
 html.MakeAll();
 // Remake the index to list only hcana Classes.  Since the
 // THa Class document pages were made, we can still click through to
 // the THa classes
 html.MakeIndex("THc");
}
//
// To install, do
// rsync -a -e ssh htmldoc/ jlabl1:/group/hallc/www/hallcweb/html/hcanadoc/
