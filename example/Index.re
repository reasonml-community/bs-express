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
let checkProperty = (req, next, property, k) => {
  let reqData = Request.asJsonObject(req);
  switch (Js.Dict.get(reqData, property)) {
  | None => next(Next.route)
  | Some(x) =>
    switch (Js.Json.decodeBoolean(x)) {
    | Some(b) when true == Js.to_bool(b) => k()
    | _ => next(Next.route)
    }
  }
};

/* same as [checkProperty] but with a list of properties */
let checkProperties = (req, next, properties, k) => {
  let rec aux = (properties) =>
    switch properties {
    | [] => k()
    | [p, ...tl] => checkProperty(req, next, p, () => aux(tl))
    };
  aux(properties)
};

/* [setProperty req property] sets the [property] in the [req] Request
   value */
let setProperty = (req, property) => {
  let reqData = Request.asJsonObject(req);
  Js.Dict.set(reqData, property, Js.Json.boolean(Js.true_))
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
Middleware.from(
  (req, _, next) => {
    setProperty(req, "middleware0");
    next(Next.middleware)
    /* call the next middleware in the processing pipeline */
  }
);

App.useWithMany(
  app,
  [|
    Middleware.from(
      (req, _, next) =>
        checkProperty(
          req,
          next,
          "middleware0",
          () => {
            setProperty(req, "middleware1");
            next(Next.middleware)
          }
        )
    ),
    Middleware.from(
      (req, _, next) =>
        checkProperties(
          req,
          next,
          ["middleware0", "middleware1"],
          () => {
            setProperty(req, "middleware2");
            next(Next.middleware)
          }
        )
    )
  |]
);

App.get(app, ~path="/") @@
Middleware.from(
  (req, res, next) => {
    let previousMiddlewares = ["middleware0", "middleware1", "middleware2"];
    checkProperties(
      req,
      next,
      previousMiddlewares,
      () => Response.sendJson(res, makeSuccessJson())
    )
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
      (req, res, next) => {
        let previousMiddlewares = ["middleware0", "middleware1", "middleware2"];
        checkProperties(
          req,
          next,
          previousMiddlewares,
          () =>
            switch (getDictString(Request.params(req), "id")) {
            | Some("123") => Response.sendJson(res, makeSuccessJson())
            | _ => next(Next.route)
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
      (req, res, next) => {
        let previousMiddlewares = ["middleware0", "middleware1", "middleware2"];
        checkProperties(
          req,
          next,
          previousMiddlewares,
          () =>
            switch (getDictString(Request.params(req), "id")) {
            | Some("123") => Response.sendJson(res, makeSuccessJson())
            | _ => next(Next.route)
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
      (req, res, next) => {
        let previousMiddlewares = ["middleware0", "middleware1", "middleware2"];
        checkProperties(
          req,
          next,
          previousMiddlewares,
          () =>
            switch (getDictString(Request.params(req), "id")) {
            | Some("123") => Response.sendJson(res, makeSuccessJson())
            | _ => next(Next.route)
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
      (req, res, next) => {
        let previousMiddlewares = ["middleware0", "middleware1", "middleware2"];
        checkProperties(
          req,
          next,
          previousMiddlewares,
          () =>
            switch (getDictString(Request.params(req), "id")) {
            | Some("123") => Response.sendJson(res, makeSuccessJson())
            | _ => next(Next.route)
            }
        )
      }
    )
  |]
);

App.get(app, ~path="/baseUrl") @@
Middleware.from(
  (req, res, next) =>
    switch (Request.baseUrl(req)) {
    | "" => Response.sendJson(res, makeSuccessJson())
    | _ => next(Next.route)
    }
);

App.get(app, ~path="/hostname") @@
Middleware.from(
  (req, res, next) =>
    switch (Request.hostname(req)) {
    | "localhost" => Response.sendJson(res, makeSuccessJson())
    | _ => next(Next.route)
    }
);

App.get(app, ~path="/ip") @@
Middleware.from(
  (req, res, next) =>
    switch (Request.ip(req)) {
    | "127.0.0.1" => Response.sendJson(res, makeSuccessJson())
    | s =>
      Js.log(s);
      next(Next.route)
    /* TODO why is it printing ::1 */
    }
);

App.get(app, ~path="/method") @@
Middleware.from(
  (req, res, next) =>
    switch (Request.httpMethod(req)) {
    | Request.Get => Response.sendJson(res, makeSuccessJson())
    | s =>
      Js.log(s);
      next(Next.route)
    }
);

App.get(app, ~path="/originalUrl") @@
Middleware.from(
  (req, res, next) =>
    switch (Request.originalUrl(req)) {
    | "/originalUrl" => Response.sendJson(res, makeSuccessJson())
    | s =>
      Js.log(s);
      next(Next.route)
    }
);

App.get(app, ~path="/path") @@
Middleware.from(
  (req, res, next) =>
    switch (Request.path(req)) {
    | "/path" => Response.sendJson(res, makeSuccessJson())
    | s =>
      Js.log(s);
      next(Next.route)
    }
);

App.get(app, ~path="/protocol") @@
Middleware.from(
  (req, res, next) =>
    switch (Request.protocol(req)) {
    | Request.Http => Response.sendJson(res, makeSuccessJson())
    | s =>
      Js.log(s);
      next(Next.route)
    }
);

App.get(app, ~path="/query") @@
Middleware.from(
  (req, res, next) =>
    switch (getDictString(Request.query(req), "key")) {
    | Some("value") => Response.sendJson(res, makeSuccessJson())
    | _ => next(Next.route)
    }
);

App.get(app, ~path="/not-found") @@
Middleware.from((_, res, _) => Response.sendStatus(res, Response.StatusCode.NotFound));

App.get(app, ~path="/error") @@
Middleware.from(
  (_, res, _) =>
    Response.sendJson(
      Response.status(res, Response.StatusCode.InternalServerError),
      makeSuccessJson()
    )
);

App.getWithMany(
  app,
  ~path="/accepts",
  [|
    Middleware.from(
      (req, _, next) =>
        switch (Request.accepts(req, [|"audio/whatever", "audio/basic"|])) {
        | Some("audio/basic") => next(Next.middleware)
        | _ => next(Next.route)
        }
    ),
    Middleware.from(
      (req, res, next) =>
        switch (Request.accepts(req, [|"text/css"|])) {
        | None => Response.sendJson(res, makeSuccessJson())
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
      (req, _, next) =>
        switch (Request.acceptsCharsets(req, [|"UTF-8", "UTF-16"|])) {
        | Some("UTF-8") => next(Next.middleware)
        | _ => next(Next.route)
        }
    ),
    Middleware.from(
      (req, res, next) =>
        switch (Request.acceptsCharsets(req, [|"UTF-16"|])) {
        | None => Response.sendJson(res, makeSuccessJson())
        | _ => next(Next.route)
        }
    )
  |]
);

App.get(app, ~path="/get") @@
Middleware.from(
  (req, res, next) =>
    switch (Request.get(req, "key")) {
    | Some("value") => Response.sendJson(res, makeSuccessJson())
    | _ => next(Next.route)
    }
);

App.get(app, ~path="/fresh") @@
Middleware.from(
  (req, res, next) =>
    if ((!) @@ Request.fresh(req)) {
      Response.sendJson(res, makeSuccessJson())
    } else {
      next(Next.route)
    }
);

App.get(app, ~path="/stale") @@
Middleware.from(
  (req, res, next) =>
    if (Request.stale(req)) {
      Response.sendJson(res, makeSuccessJson())
    } else {
      next(Next.route)
    }
);

App.get(app, ~path="/secure") @@
Middleware.from(
  (req, res, next) =>
    if ((!) @@ Request.secure(req)) {
      Response.sendJson(res, makeSuccessJson())
    } else {
      next(Next.route)
    }
);

App.get(app, ~path="/xhr") @@
Middleware.from(
  (req, res, next) =>
    if ((!) @@ Request.xhr(req)) {
      Response.sendJson(res, makeSuccessJson())
    } else {
      next(Next.route)
    }
);

App.get(app, ~path="/redir") @@
Middleware.from((_req, res, _next) => Response.redirect(res, "/redir/target"));

App.get(app, ~path="/redircode") @@
Middleware.from((_req, res, _next) => Response.redirectCode(res, 301, "/redir/target"));

App.getWithMany(app, ~path="/ocaml-exception") @@
[|
  Middleware.from((_, _, _next) => raise(Failure("Elvis has left the building!"))),
  Middleware.fromError(
    (err, _, res, _) =>
      switch err {
      | Failure(f) =>
        Response.sendString(Response.status(res, Response.StatusCode.PaymentRequired), f)
      | _ => Response.sendStatus(res, Response.StatusCode.NotFound)
      }
  )
|];

App.get(app, ~path="/promise") @@
PromiseMiddleware.from(
  (_req, res, _next) => Js.Promise.resolve(Response.sendStatus(res, Response.StatusCode.NoContent))
);

App.getWithMany(app, ~path="/failing-promise") @@
[|
  PromiseMiddleware.from((_, _, _next) => Js.Promise.reject(Not_found)),
  PromiseMiddleware.fromError(
    (_, _req, res, _next) =>
      Js.Promise.resolve(
        Response.sendString(
          Response.status(res, Response.StatusCode.InternalServerError),
          "Caught Failing Promise"
        )
      )
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