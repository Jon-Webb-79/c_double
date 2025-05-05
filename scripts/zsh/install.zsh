#!/usr/bin/zsh
# ================================================================================
# ================================================================================
# - File:    install.zsh
# - Purpose: Install/Update c_float and c_string library files
#
# Source Metadata
# - Author:  Jonathan A. Webb
# - Date:    January 11, 2025
# - Version: 1.1
# - Copyright: Copyright 2025, Jon Webb Inc.
# ================================================================================
# ================================================================================

# Check if running with sudo/root permissions
if [ "$EUID" -ne 0 ]; then
   echo "Please run as root (use sudo)"
   exit 1
fi

# Detect OS
if [[ "$OSTYPE" == "darwin"* ]]; then
   INCLUDE_DIR="/usr/local/include"
   LIB_DIR="/usr/local/lib"
elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
   INCLUDE_DIR="/usr/include"
   LIB_DIR="/usr/lib"
else
   echo "Unsupported operating system"
   exit 1
fi

# Create directories
mkdir -p "$INCLUDE_DIR"
mkdir -p "$LIB_DIR"
BACKUP_DIR="/tmp/c_libs_backup"
mkdir -p "$BACKUP_DIR"

# Function to backup and install file
install_file() {
   local src=$1
   local dest=$2
   local desc=$3
   
   # Create backup if file exists
   if [ -f "$dest" ]; then
       echo "Updating existing $desc..."
       local backup_path="$BACKUP_DIR/$(basename $dest).$(date +%Y%m%d_%H%M%S)"
       cp "$dest" "$backup_path"
       echo "Backed up to $backup_path"
   else
       echo "Installing new $desc..."
   fi
   
   # Copy new file
   cp "$src" "$dest"
   if [ $? -eq 0 ]; then
       echo "$desc installed/updated successfully"
       chmod 644 "$dest"
   else
       echo "Error installing $desc"
       return 1
   fi
   return 0
}

# Install c_float files
echo "Processing c_float library files..."
install_file "../../c_double/c_double.h" "$INCLUDE_DIR/c_double.h" "float header" || exit 1
install_file "../../c_double/c_double.c" "$LIB_DIR/c_double.c" "float source" || exit 1

# Install c_string files
echo -e "\nProcessing c_string library files..."
install_file "../../c_double/c_string.h" "$INCLUDE_DIR/c_string.h" "string header" || exit 1
install_file "../../c_double/c_string.c" "$LIB_DIR/c_string.c" "string source" || exit 1

echo -e "\nInstallation/Update completed successfully"
echo "Backups (if any) are stored in $BACKUP_DIR"
# ================================================================================
# ================================================================================
# eof
