#!/bin/bash
# chmod_all.sh - Add execute permissions to all shell scripts in the project

# Print a message about what we're doing
echo "Adding execute permissions to all shell scripts in the notepad_x project..."

# Find all .sh files in the current directory and subdirectories and make them executable
find . -name "*.sh" -type f | while read script; do
    echo "Setting executable permission on: $script"
    chmod +x "$script"
done

# Make the script itself executable (in case it wasn't run with bash directly)
chmod +x ./chmod_all.sh

# Count how many scripts were modified
count=$(find . -name "*.sh" -type f | wc -l)
echo "Done! Added execute permissions to $count shell scripts."

# List all executable shell scripts
echo -e "\nExecutable shell scripts:"
find . -name "*.sh" -type f -perm -u+x | sort