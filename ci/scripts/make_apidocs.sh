#!/usr/bin/env bash

# Remote old files
git rm docs/dev/api/*.md

# Make API doc files
lazydocs --output-path="./docs/dev/api" --overview-file="README.md" --src-base-url="../../" --no-watermark frontends/concrete-python

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

# New files?
git add docs/dev/api/*.md

echo "You might have new API-doc files to git push"
