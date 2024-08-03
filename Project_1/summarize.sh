#!/bin/bash

# Check if the user has provided an argument
if [ -z "$1" ]; then
    echo "Usage: $0 <string_to_match>"
    exit 1
fi

# Assign the argument to a variable
MATCH_STRING=$1

# Initialize an associative array to store the sums
declare -A sums

# Iterate over all files in the current directory
for file in *; do
    # Check if it's a file
    if [ -f "$file" ]; then
        # Process each line in the file
        while IFS=, read -r string number; do
            # Remove leading and trailing whitespace from string and number
            string=$(echo "$string" | sed 's/^[[:space:]]*//;s/[[:space:]]*$//')
            number=$(echo "$number" | sed 's/^[[:space:]]*//;s/[[:space:]]*$//')
            # Check if the string ends with the provided match string
            if [[ "$string" == *"$MATCH_STRING" ]]; then
                # Add the number to the corresponding sum in the associative array
                sums["$string"]=$((sums["$string"] + number))
            fi
        done < "$file"
    fi
done

# Print the results
total_sum=0
for key in "${!sums[@]}"; do
    echo "$key, ${sums[$key]}"
    total_sum=$((total_sum + sums[$key]))
done

# Print the total sum
echo "Total sum of numbers for strings ending with $MATCH_STRING: $total_sum"

