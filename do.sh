#!/usr/bin/env bash

INVOCATION_PATH="$(pwd)"
cd "$(dirname "$0")"
LIBRARY_PATH="$(pwd)"
cd "$INVOCATION_PATH"

build_library() {
    local SOURCE_FOLDER_PATH="$LIBRARY_PATH/src_c"
    ((LEN_FIND_COMMAND_PREFIX=${#SOURCE_FOLDER_PATH} + 2))
    local SOURCE_FILES=$(find "$SOURCE_FOLDER_PATH" -type f | cut -c $LEN_FIND_COMMAND_PREFIX-)

    OBJECT_FOLDER="$LIBRARY_PATH/.build/objects"
    mkdir -p $OBJECT_FOLDER

    for SOURCE_FILE in $SOURCE_FILES
    do
        # Write in bold with the arrow in blue and the '$$SOURCE_FILE' in white:
        #  ==> $SOURCE_FILE

        echo -e "\e[34m\e[1m==>\e[37m $SOURCE_FILE\e[0m"

        OBJECT_NAME="$(echo $SOURCE_FILE | tr "/" "-").o"

        gcc -c -o "$OBJECT_FOLDER/$OBJECT_NAME" "$SOURCE_FOLDER_PATH/$SOURCE_FILE" -I "$LIBRARY_PATH/inc_c"
    done

    ar -mc "$LIBRARY_PATH/.build/librr.a" "$LIBRARY_PATH/.build/objects/"*.o
}

case "$1" in
    "b")
        build_library "$@"
        ;;
esac

