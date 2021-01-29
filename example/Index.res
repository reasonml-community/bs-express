open Express

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
  let reqData = Request.asJsonObject(req)
  switch Js.Dict.get(reqData, property) {
  | None => next(Next.route, res)
  | Some(x) =>
    switch Js.Json.decodeBoolean(x) {
    | Some(b) when b => k(res)
    | _ => next(Next.route, res)
    }
  }
}

/* same as [checkProperty] but with a list of properties */
let checkProperties = (req, next, properties, k, res) => {
  let rec aux = properties =>
    switch properties {
    | list{} => k(res)
    | list{p, ...tl} => checkProperty(req, next, p, _ => aux(tl), res)
    }
  aux(properties)
}

/* [setProperty req property] sets the [property] in the [req] Request
 value */
let setProperty = (req, property, res) => {
  let reqData = Request.asJsonObject(req)
  Js.Dict.set(reqData, property, Js.Json.boolean(true))
  res
}

/* return the string value for [key], None if the key is not in [dict]
 TODO once BOption.map is released */
let getDictString = (dict, key) =>
  switch Js.Dict.get(dict, key) {
  | Some(json) => Js.Json.decodeString(json)
  | _ => None
  }

/* make a common JSON object representing success */
let makeSuccessJson = () => {
  let json = Js.Dict.empty()
  Js.Dict.set(json, "success", Js.Json.boolean(true))
  Js.Json.object_(json)
}

let app = express()
/*
  If you would like to set view engine
  App.set(app, "view engine", "pug");
 */

App.disable(app, ~name="x-powered-by")

App.useOnPath(
  app,
  ~path="/",
  Middleware.from((next, req, res) =>
    res |> setProperty(req, "middleware0") |> next(Next.middleware)
  ),
) /* call the next middleware in the processing pipeline */

App.useWithMany(
  app,
  [
    Middleware.from((next, req) =>
      checkProperty(req, next, "middleware0", res =>
        res |> setProperty(req, "middleware1") |> next(Next.middleware)
      )
    ),
    Middleware.from((next, req) =>
      checkProperties(req, next, list{"middleware0", "middleware1"}, res =>
        next(Next.middleware, setProperty(req, "middleware2", res))
      )
    ),
  ],
)

App.get(
  app,
  ~path="/",
  Middleware.from((next, req) => {
    let previousMiddlewares = list{"middleware0", "middleware1", "middleware2"}
    checkProperties(req, next, previousMiddlewares, Response.sendJson(_, makeSuccessJson()))
  }),
)

App.useOnPath(
  app,
  ~path="/static",
  {
    let options = Static.defaultOptions()
    Static.make("static", options) |> Static.asMiddleware
  },
)

App.postWithMany(
  app,
  ~path="/:id/id",
  [
    Middleware.from((next, req) => {
      let previousMiddlewares = list{"middleware0", "middleware1", "middleware2"}
      checkProperties(req, next, previousMiddlewares, res =>
        switch getDictString(Request.params(req), "id") {
        | Some("123") => Response.sendJson(res, makeSuccessJson())
        | _ => next(Next.route, res)
        }
      )
    }),
  ],
)

App.patchWithMany(
  app,
  ~path="/:id/id",
  [
    Middleware.from((next, req) => {
      let previousMiddlewares = list{"middleware0", "middleware1", "middleware2"}
      checkProperties(req, next, previousMiddlewares, res =>
        switch getDictString(Request.params(req), "id") {
        | Some("123") => Response.sendJson(res, makeSuccessJson())
        | _ => next(Next.route, res)
        }
      )
    }),
  ],
)

App.putWithMany(
  app,
  ~path="/:id/id",
  [
    Middleware.from((next, req) => {
      let previousMiddlewares = list{"middleware0", "middleware1", "middleware2"}
      checkProperties(req, next, previousMiddlewares, res =>
        switch getDictString(Request.params(req), "id") {
        | Some("123") => Response.sendJson(res, makeSuccessJson())
        | _ => next(Next.route, res)
        }
      )
    }),
  ],
)

App.deleteWithMany(
  app,
  ~path="/:id/id",
  [
    Middleware.from((next, req) => {
      let previousMiddlewares = list{"middleware0", "middleware1", "middleware2"}
      checkProperties(req, next, previousMiddlewares, res =>
        switch getDictString(Request.params(req), "id") {
        | Some("123") => Response.sendJson(res, makeSuccessJson())
        | _ => next(Next.route, res)
        }
      )
    }),
  ],
)

/* If you have set up view engine, you can uncomment this "get"
Middleware.from((_, _) =>{
   let dict: Js.Dict.t(string) = Js.Dict.empty();
   Response.render("index", dict, ());
});
*/

App.get(
  app,
  ~path="/baseUrl",
  Middleware.from((next, req) =>
    switch Request.baseUrl(req) {
    | "" => Response.sendJson(_, makeSuccessJson())
    | _ => next(Next.route)
    }
  ),
)

App.get(
  app,
  ~path="/hostname",
  Middleware.from((next, req) =>
    switch Request.hostname(req) {
    | "localhost" => Response.sendJson(_, makeSuccessJson())
    | _ => next(Next.route)
    }
  ),
)

App.get(
  app,
  ~path="/ip",
  Middleware.from((next, req) =>
    switch Request.ip(req) {
    | "127.0.0.1" => Response.sendJson(_, makeSuccessJson())
    | s =>
      Js.log(s)
      next(Next.route)
    /* TODO why is it printing ::1 */
    }
  ),
)

App.get(
  app,
  ~path="/method",
  Middleware.from((next, req) =>
    switch Request.httpMethod(req) {
    | Request.Get => Response.sendJson(_, makeSuccessJson())
    | s =>
      Js.log(s)
      next(Next.route)
    }
  ),
)

App.get(
  app,
  ~path="/originalUrl",
  Middleware.from((next, req) =>
    switch Request.originalUrl(req) {
    | "/originalUrl" => Response.sendJson(_, makeSuccessJson())
    | s =>
      Js.log(s)
      next(Next.route)
    }
  ),
)

App.get(
  app,
  ~path="/path",
  Middleware.from((next, req) =>
    switch Request.path(req) {
    | "/path" => Response.sendJson(_, makeSuccessJson())
    | s =>
      Js.log(s)
      next(Next.route)
    }
  ),
)

App.get(
  app,
  ~path="/protocol",
  Middleware.from((next, req) =>
    switch Request.protocol(req) {
    | Request.Http => Response.sendJson(_, makeSuccessJson())
    | s =>
      Js.log(s)
      next(Next.route)
    }
  ),
)

App.get(
  app,
  ~path="/query",
  Middleware.from((next, req) =>
    switch getDictString(Request.query(req), "key") {
    | Some("value") => Response.sendJson(_, makeSuccessJson())
    | _ => next(Next.route)
    }
  ),
)

App.get(
  app,
  ~path="/not-found",
  Middleware.from((_, _) => Response.sendStatus(_, Response.StatusCode.NotFound)),
)

App.get(
  app,
  ~path="/error",
  Middleware.from((_, _, res) =>
    res
    ->Response.status(Response.StatusCode.InternalServerError)
    ->Response.sendJson(makeSuccessJson())
  ),
)

App.getWithMany(
  app,
  ~path="/accepts",
  [
    Middleware.from((next, req) =>
      switch Request.accepts(req, ["audio/whatever", "audio/basic"]) {
      | Some("audio/basic") => next(Next.middleware)
      | _ => next(Next.route)
      }
    ),
    Middleware.from((next, req) =>
      switch Request.accepts(req, ["text/css"]) {
      | None => Response.sendJson(_, makeSuccessJson())
      | _ => next(Next.route)
      }
    ),
  ],
)

let \">>" = (f, g, x) => x |> f |> g

App.getWithMany(
  app,
  ~path="/accepts-charsets",
  [
    Middleware.from((next, req) =>
      switch Request.acceptsCharsets(req, ["UTF-8", "UTF-16"]) {
      | Some("UTF-8") => next(Next.middleware)
      | _ => next(Next.route)
      }
    ),
    Middleware.from((next, req) =>
      switch Request.acceptsCharsets(req, ["UTF-16"]) {
      | None => Response.sendJson(_, makeSuccessJson())
      | _ => next(Next.route)
      }
    ),
  ],
)

App.get(
  app,
  ~path="/get",
  Middleware.from((next, req) =>
    switch Request.get(req, "key") {
    | Some("value") => Response.sendJson(_, makeSuccessJson())
    | _ => next(Next.route)
    }
  ),
)

App.get(
  app,
  ~path="/fresh",
  Middleware.from((next, req) =>
    if !Request.fresh(req) {
      Response.sendJson(_, makeSuccessJson())
    } else {
      next(Next.route)
    }
  ),
)

App.get(
  app,
  ~path="/stale",
  Middleware.from((next, req) =>
    if Request.stale(req) {
      Response.sendJson(_, makeSuccessJson())
    } else {
      next(Next.route)
    }
  ),
)

App.get(
  app,
  ~path="/secure",
  Middleware.from((next, req) =>
    if !Request.secure(req) {
      Response.sendJson(_, makeSuccessJson())
    } else {
      next(Next.route)
    }
  ),
)

App.get(
  app,
  ~path="/xhr",
  Middleware.from((next, req) =>
    if !Request.xhr(req) {
      Response.sendJson(_, makeSuccessJson())
    } else {
      next(Next.route)
    }
  ),
)

App.get(app, ~path="/redir", Middleware.from((_, _) => Response.redirect(_, "/redir/target")))

App.get(
  app,
  ~path="/redircode",
  Middleware.from((_, _) => Response.redirectCode(_, 301, "/redir/target")),
)

App.getWithMany(
  app,
  ~path="/ocaml-exception",
  [
    Middleware.from((_, _, _next) => raise(Failure("Elvis has left the building!"))),
    Middleware.fromError((_, err, _, res) =>
      switch err {
      | Failure(f) =>
        res->Response.status(Response.StatusCode.PaymentRequired)->Response.sendString(f)
      | _ => res->Response.sendStatus(Response.StatusCode.NotFound)
      }
    ),
  ],
)

App.get(
  app,
  ~path="/promise",
  PromiseMiddleware.from((_req, _next, res) =>
    res->Response.sendStatus(Response.StatusCode.NoContent)->Js.Promise.resolve
  ),
)

App.getWithMany(
  app,
  ~path="/failing-promise",
  [
    PromiseMiddleware.from((_, _, _next) => Js.Promise.reject(Not_found)),
    PromiseMiddleware.fromError((_, _req, _next, res) =>
      res
      ->Response.status(Response.StatusCode.InternalServerError)
      ->Response.sendString("Caught Failing Promise")
      ->Js.Promise.resolve
    ),
  ],
)

let router1 = router()

Router.get(router1, ~path="/123", Middleware.from((_, _) => Response.sendStatus(_, Created)))

App.useRouterOnPath(app, ~path="/testing/testing", router1)

let router2 = router(~caseSensitive=true, ~strict=true, ())

Router.get(router2, ~path="/Case-sensitive", Middleware.from((_, _) => Response.sendStatus(_, Ok)))

Router.get(router2, ~path="/strict/", Middleware.from((_, _) => Response.sendStatus(_, Ok)))

App.useRouterOnPath(app, ~path="/router-options", router2)

App.param(
  app,
  ~name="identifier",
  Middleware.from((_next, _req) => Response.sendStatus(_, Created)),
)

App.get(
  app,
  ~path="/param-test/:identifier",
  Middleware.from((_next, _req) => Response.sendStatus(_, BadRequest)),
)

App.get(
  app,
  ~path="/cookie-set-test",
  Middleware.from((_next, _req, res) =>
    res
    ->Response.cookie(~name="test-cookie", Js.Json.string("cool-cookie"))
    ->Response.sendStatus(Ok)
  ),
)

App.get(
  app,
  ~path="/cookie-clear-test",
  Middleware.from((_next, _req, res) =>
    res->Response.clearCookie(~name="test-cookie2", ())->Response.sendStatus(Ok)
  ),
)

App.get(
  app,
  ~path="/response-set-header",
  Middleware.from((_, _, res) =>
    res->Response.setHeader("X-Test-Header", "Set")->Response.sendStatus(Response.StatusCode.Ok)
  ),
)

let router3 = router(~caseSensitive=true, ~strict=true, ())

open ByteLimit

Router.use(router3, Middleware.json(~limit=5.0 |> mb, ()))

Router.use(router3, Middleware.urlencoded(~extended=true, ()))

module Body = {
  type payload = {"number": int}
  let jsonDecoder = json => {
    open Json.Decode
    {"number": json |> field("number", int)}
  }
  let urlEncodedDecoder = dict =>
    {
      "number": Js.Dict.unsafeGet(dict, "number") |> int_of_string,
    }
  let encoder = body => {
    open Json.Encode
    object_(list{("number", body["number"] |> int)})
  }
}

let raiseIfNone = x =>
  switch x {
  | Some(value) => value
  | None => Js.Exn.raiseError("Body is none")
  }

Router.post(
  router3,
  ~path="/json-doubler",
  Middleware.from((_next, req, res) => {
    let json = req->Request.bodyJSON->raiseIfNone->Body.jsonDecoder
    Response.sendJson(res, {"number": json["number"] * 2}->Body.encoder)
  }),
)

Router.post(
  router3,
  ~path="/urlencoded-doubler",
  Middleware.from((_next, req, res) => {
    let decoded = req->Request.bodyURLEncoded->raiseIfNone->Body.urlEncodedDecoder
    Response.sendJson(res, {"number": decoded["number"] * 2}->Body.encoder)
  }),
)

App.useRouterOnPath(app, ~path="/builtin-middleware", router3)

let router4 = router(~caseSensitive=true, ~strict=true, ())

Router.use(router4, Middleware.text())

Router.post(
  router4,
  ~path="/text-body",
  Middleware.from((_next, req, res) =>
    Response.sendString(res, Request.bodyText(req)->raiseIfNone)
  ),
)

App.useRouterOnPath(app, ~path="/router4", router4)

let onListen = e =>
  switch e {
  | exception Js.Exn.Error(e) => {
      Js.log(e)
      Node.Process.exit(1)
    }
  | _ => Js.log("Listening at http://127.0.0.1:3000")
  }

let server = App.listen(app, ~port=3000, ~onListen, ())

let countRequests = server => {
  let count = ref(0)
  HttpServer.on(server, #request((_, _) => count := count.contents + 1))
  () => {
    let result = count.contents
    count := -1
    result
  }
}

let getRequestsCount = countRequests(server)

App.post(
  app,
  ~path="/get-request-count",
  Middleware.from((_, _, res) =>
    Response.sendString(
      res,
      "The server has been called " ++ (string_of_int(getRequestsCount()) ++ " times."),
    )
  ),
)

/* Other examples are
   App.listen app ();
   App.listen app port::1000 ();
   App.listen app port::1000 onListen::(fun e => Js.log e) ();
 */
/* -- Test the server --
   npm run start && cd tests && ./test.sh
 */
