# The input JSON file
$input_file = "utilities/version/version.json"

# Parse the JSON file to extract the constants
$json = Get-Content $input_file | ConvertFrom-Json
$major = $json.RB_VERSION_MAJOR
$minor = $json.RB_VERSION_MINOR
$rev = $json.RB_VERSION_REV

# Echo the version string
echo "$major.$minor.$rev"
