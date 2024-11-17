#!/usr/bin/bash

Source_Files=( "main.cpp" )
Object_Files=()
Compiler=""

# search for valid compiler. Print nothing if clang is found w/ support for c++17 since that's our ideal. Complain if g++ works, since we really only want to support clang++.
# Then fail otherwise, since we can't build for any other compiler right now. Could maybe support msvc at some point w/ git bash, but ugghh.
if [[ ! -z $(command -v "clang++") ]]; then
    if [[ -z $(clang++ --std=c++terriblearg -c src/main.cpp 2>&1 | grep c++17 ) ]] ; then # this check is compiler dependent. Clang emits correct/supported standards when given a bad one.
        echo "Installed Clang version does not support c++17"
    else
        Compiler="clang++"
    fi
else
    echo "clang++ not found. Looking for g++."
fi

if [[ -z "$Compiler" && ! -z $(command -v "g++") ]] ; then
    if [[ -z $(g++ -v --help 2> /dev/null | sed -n '/^ *-std=\([^<][^ ]\+\).*/ {s//\1/p}' | grep c++17) ]] ; then # THX https://stackoverflow.com/users/616460/jason-c with https://stackoverflow.com/questions/34836775/compiler-standards-support-c11-c14-c17
        echo "Installed g++ version does not support c++17"
    else
        Compiler="g++"
        echo "Found g++, building."
    fi
elif [[ -z "$Compiler" ]] ; then
    echo "g++ not found. Giving up."
fi

if [[ -z $Compiler ]] ; then
    echo "Could not find suitable compiler"
    exit
fi


# creates the appropriate folders before object files, so that the compiler can actually put them in said folder. Some compilers won't create folders, even if specified in the output filename.
# then make object files.
# then link object files into an executable.
mkdir -p obj
for file in "${Source_Files[@]}" ; do
    # can probably actually handle spaces. Will test later and remove this silly check.
    if [[ $file =~ [[:space:]]+ ]] ; then
        echo "File path: \"$file\" Contains a space and should not."
        exit 1
    fi
    
    object_file="obj/src/$(echo $file | sed -e 's/\.cpp/\.o/')"
    file="src/$file"
    
    path="$(dirname $file)"
    #echo "$file ---> $object_file"
    Object_Files+=( "${object_file}" )
    mkdir -p "obj/$path" # otherwise clang++/g++ complain about non-existing directory
    $Compiler "$file" -c -o "${object_file}" -std=c++17
done

for object in "${Object_Files[@]}" ; do
    file_glob+=" $object"
done

$Compiler $file_glob -o "brain-stem"
