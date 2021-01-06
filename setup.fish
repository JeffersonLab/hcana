#!/usr/bin/fish

function update_path -d "Remove argv[2]argv[3] from argv[1] if argv[2], and prepend argv[4]"
   # Assert that we got enough arguments
   if test (count $argv) -ne 4
      echo "update_path: needs 4 arguments but have " (count $argv)
      return 1
   end

   set var $argv[1]

   set newpath $argv[4]
   for el in $$var
      if test "$argv[2]" = ""; or not test "$el" = "$argv[2]$argv[3]"
         set newpath $newpath $el
      end
   end

   set -xg $var $newpath
end


if set -q HCANALYZER
  set old_hcanalyzer $HCANALYZER
end

if set -q ANALYZER
  set old_analyzer $ANALYZER
end
set -x HCANALYZER (realpath (dirname (status -f)))
set -x ANALYZER $HCANALYZER/podd

update_path LD_LIBRARY_PATH "$old_hcanalyzer" "" $HCANALYZER
update_path LD_LIBRARY_PATH "$old_analyzer" "$HCANALYZER" $ANALYZER
update_path DYLD_LIBRARY_PATH "$old_hcanalyzer" "" $HCANALYZER
update_path DYLD_LIBRARY_PATH "$old_analyzer" "$HCANALYZER" $ANALYZER

