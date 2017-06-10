/** Binding for the cookie-parser library */

let make : secret::string? => unit => Express.Middleware.t;
/** [make ::secret ()] returns a middleware value to be used with 
    [Express.App] functions. */
