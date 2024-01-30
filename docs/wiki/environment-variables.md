Config-cxx uses a handful of environment variables for its own configuration. It can also use custom environment variables to override your app's configuration. In both cases, these are generally exported in your shell before loading the app, but can also be supplied on the command line or in your app bootstrap.

Example exporting to the O/S before loading your app:

```
cd /my/application
export CXX_ENV=stage
./executable
```

### CXX_ENV

This variable contains the name of your application's deployment environment, representing the `{deployment}` when determining config file loading order. Common values include `dev`, `stage`, `production`, etc. to fit your deployment strategy.

### CXX_CONFIG_DIR

This contains the path to the directory containing your configuration files. It can be direct path from the root, or relative path if they begin with `./` or `../`. The default `CXX_CONFIG_DIR` is the `/config` directory under the <i>current working directory</i>, usually your application root.

### Custom Environment Variables

To enable custom environment variables, create a configuration file, `config/custom-environment-variables.json` mapping the environment variable names into your configuration structure. For example:

```
// config/custom-environment-variables.json
{
  "Customer": {
    "dbConfig": {
      "host": "PROD_SERVER"
    },
    "credit": {
      "initialDays": "CR_ID"
    }
  }
}
```
...would cause `config-cxx` to check for the environment variables `PROD_SERVER` and `CR_ID`. If they exist, they would override the values for Customer.dbConfig.host, and Customer.credit.initialDays in your configuration.

<b>Precedence</b>: Custom environment variables override all configuration files, including local.json.
