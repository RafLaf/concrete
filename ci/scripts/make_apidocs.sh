#!/usr/bin/env bash

# In this script, we:
#   create a fresh directory
#   create a fresh venv
#   download the last CP
#   make API docs for it
#   git add the corresponding md files

FRESH_DIRECTORY="tempdirectoryforapidocs"

# Remote old files
rm docs/dev/api/*.md

set -e

# Make a new fresh venv, and install the last public CP there (hence, the --isolated to not take
# nightlies)
rm -rf "$FRESH_DIRECTORY"
mkdir "$FRESH_DIRECTORY"
cd "$FRESH_DIRECTORY"
python3 -m venv .venvtrash
source .venvtrash/bin/activate
pip install concrete-python --index-url https://pypi.org/simple --isolated
pip install lazydocs

# Make API doc files
.venvtrash/bin/lazydocs --output-path="../docs/dev/api" --overview-file="README.md" --src-base-url="../../" --no-watermark concrete
cd -

# Add the files in the summary
FILES=$(cd docs && find dev/api -name "*.md")

TMP_FILE=$(mktemp /tmp/apidocs.XXXXXX)
rm -rf "$TMP_FILE"
touch "$TMP_FILE"

for f in $FILES
do
    filename=$(echo "$f" | rev | cut -d '/' -f 1 | rev)

    echo "  - [$filename]($f)" >> "$TMP_FILE"
done

FINAL_FILE="docs/SUMMARY.md"
NEW_FINAL_FILE="docs/SUMMARY.md.tmp"

grep "<!-- auto-created, do not edit, begin -->" $FINAL_FILE -B 100000 > $NEW_FINAL_FILE
sort "$TMP_FILE" | grep -v "\[README.md\]" >> $NEW_FINAL_FILE
grep "<!-- auto-created, do not edit, end -->" $FINAL_FILE -A 100000 >> $NEW_FINAL_FILE

mv $NEW_FINAL_FILE $FINAL_FILE

rm -rf "$FRESH_DIRECTORY"

# New files?
echo "Warning. You might have new API-doc files to git add & push, don't forget"

# Fixing the path issues, to point on files in GitHub
WHICH_PYTHON_VERSION=$(python3 --version | cut -f 2 -d " " | cut -f 1-2 -d ".")
sed -i "" -e "s@../../$FRESH_DIRECTORY/.venvtrash/lib/python$WHICH_PYTHON_VERSION/site-packages/@../../../compilers/concrete-compiler/compiler/lib/Bindings/Python/@g" docs/dev/api/*.md

# Fixing the links in README.md, which fails (missing .'s for some reason): remove the #headers
sed -i"" -e "s@.md#module-.*)@.md)@g" docs/dev/api/README.md

# FIXME: remove this once the PR has been merged once
sed -i "" -e "s@https://github.com/zama-ai/concrete-compiler-internal/blob/main/LICENSE.txt@https://github.com/zama-ai/concrete/blob/main/LICENSE.txt@g" ./docs/dev/api/concrete.lang.dialects.md ./docs/dev/api/concrete.compiler.md ./docs/dev/api/concrete.lang.md


