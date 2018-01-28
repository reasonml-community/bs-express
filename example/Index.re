open Express;

/* The tests below relies upon the ability to store in the Request
      objects abritrary JSON properties.

      Each middleware will both check that previous middleware
      have been called by making properties exists in the Request object and
      upon success will themselves adds another property to the Request.

   */
/* [checkProperty req next property k] makes sure [property] is
   present in [req]. If success then [k()] is invoked, otherwise
   [Next.route] is called with next */
let checkProperty = (req, next, property, k, res) => {
  let reqData = Request.asJsonObject(req);
  switch (Js.Dict.get(reqData, property)) {
  | None => next(Next.route, res)
  | Some(x) =>
    switch (Js.Json.decodeBoolean(x)) {
    | Some(b) when true == Js.to_bool(b) => k(res)
    | _ => next(Next.route, res)
    }
  }
};

/* same as [checkProperty] but with a list of properties */
let checkProperties = (req, next, properties, k, res) => {
  let rec aux = (properties) =>
    switch properties {
    | [] => k(res)
    | [p, ...tl] => checkProperty(req, next, p, (_) => aux(tl), res)
    };
  aux(properties)
};

/* [setProperty req property] sets the [property] in the [req] Request
   value */
let setProperty = (req, property, res) => {
  let reqData = Request.asJsonObject(req);
  Js.Dict.set(reqData, property, Js.Json.boolean(Js.true_));
  res
};

/* return the string value for [key], None if the key is not in [dict]
   TODO once BOption.map is released */
let getDictString = (dict, key) =>
  switch (Js.Dict.get(dict, key)) {
  | Some(json) => Js.Json.decodeString(json)
  | _ => None
  };

/* make a common JSON object representing success */
let makeSuccessJson = () => {
  let json = Js.Dict.empty();
  Js.Dict.set(json, "success", Js.Json.boolean(Js.true_));
  Js.Json.object_(json)
};

let app = express();

App.useOnPath(app, ~path="/") @@
Middleware.from((req, next, res) => res |> setProperty(req, "middleware0") |> next(Next.middleware)) /* call the next middleware in the processing pipeline */;

App.useWithMany(
  app,
  [|
    Middleware.from(
      (req, next) =>
        checkProperty(
          req,
          next,
          "middleware0",
          res => 
            res 
            |> setProperty(req, "middleware1") 
            |> next(Next.middleware)
        )
    ),
    Middleware.from(
      (req, next) =>
        checkProperties(
          req,
          next,
          ["middleware0", "middleware1"],
          res => next(Next.middleware,setProperty(req, "middleware2", res))          
        )
    )
  |]
);

App.get(app, ~path="/") @@
Middleware.from(
  (req, next) => {
    let previousMiddlewares = ["middleware0", "middleware1", "middleware2"];
    checkProperties(req, next, previousMiddlewares, Response.sendJson(makeSuccessJson()))
  }
);

App.useOnPath(
  app,
  ~path="/static",
  {
    let options = Static.defaultOptions();
    Static.make("static", options) |> Static.asMiddleware
  }
);

App.postWithMany(
  app,
  ~path="/:id/id",
  [|
    Middleware.from(
      (req, next) => {
        let previousMiddlewares = ["middleware0", "middleware1", "middleware2"];
        checkProperties(
          req,
          next,
          previousMiddlewares,
          (res) =>
            switch (getDictString(Request.params(req), "id")) {
            | Some("123") => Response.sendJson(makeSuccessJson(), res)
            | _ => next(Next.route, res)
            }
        )
      }
    )
  |]
);

App.patchWithMany(
  app,
  ~path="/:id/id",
  [|
    Middleware.from(
      (req, next) => {
        let previousMiddlewares = ["middleware0", "middleware1", "middleware2"];
        checkProperties(
          req,
          next,
          previousMiddlewares,
          (res) =>
            switch (getDictString(Request.params(req), "id")) {
            | Some("123") => Response.sendJson(makeSuccessJson(), res)
            | _ => next(Next.route, res)
            }
        )
      }
    )
  |]
);

App.putWithMany(
  app,
  ~path="/:id/id",
  [|
    Middleware.from(
      (req, next) => {
        let previousMiddlewares = ["middleware0", "middleware1", "middleware2"];
        checkProperties(
          req,
          next,
          previousMiddlewares,
          (res) =>
            switch (getDictString(Request.params(req), "id")) {
            | Some("123") => Response.sendJson(makeSuccessJson(), res)
            | _ => next(Next.route, res)
            }
        )
      }
    )
  |]
);

App.deleteWithMany(
  app,
  ~path="/:id/id",
  [|
    Middleware.from(
      (req, next) => {
        let previousMiddlewares = ["middleware0", "middleware1", "middleware2"];
        checkProperties(
          req,
          next,
          previousMiddlewares,
          (res) =>
            switch (getDictString(Request.params(req), "id")) {
            | Some("123") => Response.sendJson(makeSuccessJson(), res)
            | _ => next(Next.route, res)
            }
        )
      }
    )
  |]
);

App.get(app, ~path="/baseUrl") @@
Middleware.from(
  (req, next) =>
    switch (Request.baseUrl(req)) {
    | "" => Response.sendJson(makeSuccessJson())
    | _ => next(Next.route)
    }
);

App.get(app, ~path="/hostname") @@
Middleware.from(
  (req, next) =>
    switch (Request.hostname(req)) {
    | "localhost" => Response.sendJson(makeSuccessJson())
    | _ => next(Next.route)
    }
);

App.get(app, ~path="/ip") @@
Middleware.from(
  (req, next) =>
    switch (Request.ip(req)) {
    | "127.0.0.1" => Response.sendJson(makeSuccessJson())
    | s =>
      Js.log(s);
      next(Next.route)
    /* TODO why is it printing ::1 */
    }
);

App.get(app, ~path="/method") @@
Middleware.from(
  (req, next) =>
    switch (Request.httpMethod(req)) {
    | Request.Get => Response.sendJson(makeSuccessJson())
    | s =>
      Js.log(s);
      next(Next.route)
    }
);

App.get(app, ~path="/originalUrl") @@
Middleware.from(
  (req, next) =>
    switch (Request.originalUrl(req)) {
    | "/originalUrl" => Response.sendJson(makeSuccessJson())
    | s =>
      Js.log(s);
      next(Next.route)
    }
);

App.get(app, ~path="/path") @@
Middleware.from(
  (req, next) =>
    switch (Request.path(req)) {
    | "/path" => Response.sendJson(makeSuccessJson())
    | s =>
      Js.log(s);
      next(Next.route)
    }
);

App.get(app, ~path="/protocol") @@
Middleware.from(
  (req, next) =>
    switch (Request.protocol(req)) {
    | Request.Http => Response.sendJson(makeSuccessJson())
    | s =>
      Js.log(s);
      next(Next.route)
    }
);

App.get(app, ~path="/query") @@
Middleware.from(
  (req, next) =>
    switch (getDictString(Request.query(req), "key")) {
    | Some("value") => Response.sendJson(makeSuccessJson())
    | _ => next(Next.route)
    }
);

App.get(app, ~path="/not-found") @@
Middleware.from((_, _) => Response.sendStatus(Response.StatusCode.NotFound));

App.get(app, ~path="/error") @@
Middleware.from(
  (_, _, res) =>
    res
    |> Response.status(Response.StatusCode.InternalServerError)
    |> Response.sendJson(makeSuccessJson())
);

App.getWithMany(
  app,
  ~path="/accepts",
  [|
    Middleware.from(
      (req, next) =>
        switch (Request.accepts(req, [|"audio/whatever", "audio/basic"|])) {
        | Some("audio/basic") => next(Next.middleware)
        | _ => next(Next.route)
        }
    ),
    Middleware.from(
      (req, next) =>
        switch (Request.accepts(req, [|"text/css"|])) {
        | None => Response.sendJson(makeSuccessJson())
        | _ => next(Next.route)
        }
    )
  |]
);

App.getWithMany(
  app,
  ~path="/accepts-charsets",
  [|
    Middleware.from(
      (req, next) =>
        switch (Request.acceptsCharsets(req, [|"UTF-8", "UTF-16"|])) {
        | Some("UTF-8") => next(Next.middleware)
        | _ => next(Next.route)
        }
    ),
    Middleware.from(
      (req, next) =>
        switch (Request.acceptsCharsets(req, [|"UTF-16"|])) {
        | None => Response.sendJson(makeSuccessJson())
        | _ => next(Next.route)
        }
    )
  |]
);

App.get(app, ~path="/get") @@
Middleware.from(
  (req, next) =>
    switch (Request.get(req, "key")) {
    | Some("value") => Response.sendJson(makeSuccessJson())
    | _ => next(Next.route)
    }
);

App.get(app, ~path="/fresh") @@
Middleware.from(
  (req, next) =>
    if ((!) @@ Request.fresh(req)) {
      Response.sendJson(makeSuccessJson())
    } else {
      next(Next.route)
    }
);

App.get(app, ~path="/stale") @@
Middleware.from(
  (req, next) =>
    if (Request.stale(req)) {
      Response.sendJson(makeSuccessJson())
    } else {
      next(Next.route)
    }
);

App.get(app, ~path="/secure") @@
Middleware.from(
  (req, next) =>
    if ((!) @@ Request.secure(req)) {
      Response.sendJson(makeSuccessJson())
    } else {
      next(Next.route)
    }
);

App.get(app, ~path="/xhr") @@
Middleware.from(
  (req, next) =>
    if ((!) @@ Request.xhr(req)) {
      Response.sendJson(makeSuccessJson())
    } else {
      next(Next.route)
    }
);

App.get(app, ~path="/redir") @@ Middleware.from((_, _) => Response.redirect("/redir/target"));

App.get(app, ~path="/redircode") @@
Middleware.from((_, _) => Response.redirectCode(301, "/redir/target"));

App.getWithMany(app, ~path="/ocaml-exception") @@
[|
  Middleware.from((_, _, _next) => raise(Failure("Elvis has left the building!"))),
  Middleware.fromError(
    (err, _, _, res) =>
      switch err {
      | Failure(f) =>
        res |> Response.status(Response.StatusCode.PaymentRequired) |> Response.sendString(f)
      | _ => res |> Response.sendStatus(Response.StatusCode.NotFound)
      }
  )
|];

App.get(app, ~path="/promise") @@
PromiseMiddleware.from(
  (_req, _next, res) => res |> Response.sendStatus(Response.StatusCode.NoContent) |> Js.Promise.resolve
);

App.getWithMany(app, ~path="/failing-promise") @@
[|
  PromiseMiddleware.from((_, _, _next) => Js.Promise.reject(Not_found)),
  PromiseMiddleware.fromError(
    (_, _req, _next, res) =>
      res
      |> Response.status(Response.StatusCode.InternalServerError)
      |> Response.sendString("Caught Failing Promise")
      |> Js.Promise.resolve
  )
|];

let onListen = (port, e) =>
  switch e {
  | exception (Js.Exn.Error(e)) =>
    Js.log(e);
    Node.Process.exit(1)
  | _ => Js.log @@ "Listening at http://127.0.0.1:" ++ string_of_int(port)
  };

App.listen(app, ~onListen=onListen(3000), ());
/* Other examples are
   App.listen app ();
   App.listen app port::1000 ();
   App.listen app port::1000 onListen::(fun e => Js.log e) ();
   */
/* -- Test the server --
   npm run start && cd tests && ./test.sh
   */