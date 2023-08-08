#!/bin/sh

# Copy main library file
source_dir_LIB="./build"
dest_dir_LIB="/usr/local/lib"

# Check if the file exists
if [ -e "$source_dir_LIB/libjconfigloader.a" ]; then
    # Copy the file
    sudo cp "$source_dir_LIB/libjconfigloader.a" "$dest_dir_LIB/"

    # Check if the copy was successful
    if [ $? -eq 0 ]; then
        echo "File libjconfigloader.a copied successfully to $dest_dir_LIB/"
    else
        echo "Failed to copy libjconfigloader.a"
    fi
else
    echo "File libjconfigloader.a does not exist in $source_dir_LIB"
fi

# ----

# Copy header files

# Source and destination directories
source_dir_HEAD="./headers"
dest_dir_HEAD="/usr/include/jconfigloader"

# Create the destination directory if it doesn't exist
sudo mkdir -p "$dest_dir_HEAD"

# Get the list of file names before copying
files_before=$(find "$dest_dir_HEAD" -type f -exec basename {} \;)

# Copy files from source to destination
sudo cp -r "$source_dir_HEAD"/* "$dest_dir_HEAD/"

# Get the list of file names after copying
files_after=$(find "$dest_dir_HEAD" -type f -exec basename {} \;)

# Check if all files were copied
all_copied=true
for file_before in $files_before; do
    found=false
    for file_after in $files_after; do
        if [ "$file_before" = "$file_after" ]; then
            found=true
            break
        fi
    done
    if [ "$found" = "false" ]; then
        echo "File $file_before was not copied correctly."
        all_copied=false
    fi
done

if [ "$all_copied" = "true" ]; then
    echo "All files copied successfully to $dest_dir_HEAD/"
else
    echo "Some files might not have been copied correctly."
fi

