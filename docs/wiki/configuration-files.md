## Config Directory

Confix-cxx reads configuration files in the `./config` directory for the running process, typically the application root. This can be overridden by setting the `$CXX_CONFIG_DIR` environment variable to the directory containing your configuration files.

`$CXX_CONFIG_DIR` can be a full path from your root directory, or a relative path from the process if the value begins with `./` or `../`.

## File Load Order

Files in the config directory are loaded in the following order:

```
default.EXT
{deployment}.EXT
local.EXT
local-{deployment}.EXT
custom-environment-variables.EXT
```

Where

- `EXT` can be .yml, .yaml or .json depending on the format you prefer (see below).
- `{deployment}` is the deployment name, from the `$CXX_ENV` environment variable.


The `default.EXT` file is designed to contain all configuration parameters from which other files may overwrite. Overwriting is done on a parameter by parameter basis, so subsequent files contain only the parameters unique for that override.

`{deployment}` files allow you to tune configurations for a particular server or deployment. These files are designed to live along with other files in your version control system.


## `local` files

The `local` files are intended to not be tracked in your version control system. External configuration management tools can write these files upon application deployment, before application loading.

The best practice for using `local` files is avoid a global `local.EXT` file that would affect all `{deployment}` cases. Instead, choose `local-{deployment}.EXT`. With this design you can avoid a case where tests pass locally due to local overrides but don't pass after you push. Choose a local file variant that is not used for your testing context!

## Default CXX_ENV

If `CXX_ENV` is not set in the environment, a default value of `development` is used.

## Environment variables
Custom environment variables override all configuration files, including local.json. Only command line options take precedence over them.


## File Formats

The following file formats are supported, recognized by their file extension.

### Javascript Object Notation - .json
Files ending in .json are parsed in JSON format. Example:

```
{
  "Customer": {
    "dbConfig": {
      "host": "localhost",
      "port": 5984,
      "dbName": "customers"
    },
    "credit": {
      "initialLimit": 100,
      "initialDays": 1
    }
  }
}
```
