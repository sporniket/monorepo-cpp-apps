#!/bin/bash

# generate a project descriptor with all the files in session for geany
# @param 1 : path to project file name

if [ $# -lt 1 ]; then
  echo "Usage: generate-geany-project.sh <path_to_project_file>"
  echo
  echo "e.g.: generate-geany-project.sh ../full-project.geany"
  exit -1
fi

echo "generating $1..."
cat << EOF > "$1"
[editor]
line_wrapping=false
line_break_column=72
auto_continue_multiline=true

[file_prefs]
final_new_line=true
ensure_convert_new_lines=false
strip_trailing_spaces=false
replace_tabs=false

[indentation]
indent_width=4
indent_type=1
indent_hard_tab_width=8
detect_indent=false
detect_indent_width=false
indent_mode=2

[project]
name=monorepo-cpp-apps
base_path=/home/dsporn/Projects/zz-apps/zz-cpp/monorepo-cpp-apps
description=

[long line marker]
long_line_behaviour=1
long_line_column=72

[files]
current_page=11
EOF

### --- BEGIN generate the list of session files (all the files) ---
declare -i findex=0

for f in $(find $(pwd) -type f | grep -v "/.git/" | grep -v ".git\$"| grep -v "/build/"); do
  fextension=${f##*.}
  fencoded="${f//\//%2F}"
  ftype=""
  case $fextension in
    md) ftype="Markdown";;
    cpp) ftype="C++";;
    hpp) ftype="C++";;
  esac
  if [ -z "$ftype" ]; then
    case $(basename "$f") in
      CMakeLists.txt) ftype="CMake"
    esac
  fi
  echo "FILE_NAME_${findex}=0;${ftype};0;EUTF-8;1;1;0;${fencoded};O;4" >> "$1"
  ((++findex))
done
### --- END generate the list of session files (all the files) ---

cat << EOF >> "$1"

[VTE]
last_dir=/home/dsporn
EOF

cat "$1"

