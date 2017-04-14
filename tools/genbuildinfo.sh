#!/bin/bash
# Utility script to generate the buildinfo_current header. Generates a header
# with macro definitions about the current git branch and commit, as well as
# a build number that is incremented with each success build.
#
# The header is written to stdout

INFOFILE=.buildinfo

readconfig() {
    while IFS= read line; do
        config+=($line)
    done < "$INFOFILE"
}


writeconfig() {
    rm "$INFOFILE"
    for i in ${config[@]}; do
        echo $i >> "$INFOFILE"
    done
}



if [ $# -eq 1 ] && [ $1 = "-i" ]; then
    config=()
    readconfig
    # increment the build number and store it
    config[2]=$((config[2]+1))
    writeconfig
else

    curBranch=`git rev-parse --abbrev-ref HEAD`
    curCommit=`git rev-parse --short HEAD`

    if [ -f "$INFOFILE" ]; then
        config=()
        readconfig
        if [ "$config[0]" != "$curBranch" ] || [ "$config[1]" != "$curCommit" ]; then
            config[0]=0
            writeconfig
        fi
    else
        config=("$curBranch" "$curCommit" "0")
        touch "$INFOFILE"
        writeconfig
    fi

cat <<EOF
-DBUILDINFO_BRANCH=\"${config[0]}\" \
-DBUILDINFO_COMMIT=\"${config[1]}\" \
-DBUILDINFO_BUILDNUM=\"${config[2]}\"

EOF

fi