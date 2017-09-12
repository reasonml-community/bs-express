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
let checkProperty req next property k => {
  let reqData = Request.asJsonObject req;
  switch (Js.Dict.get reqData property) {
    | None => next Next.route
    | Some x => {
      switch (Js.Json.decodeBoolean x) {
        | Some b when (b == Js.true_) => k ();
        | _ => next Next.route;
      };
    };
  };
};

/* same as [checkProperty] but with a list of properties */
let checkProperties req next properties k => {
  let rec aux properties => {
    switch properties {
      | [] => k ();
      | [p, ...tl] => checkProperty req next p (fun () => aux tl);
    };
  };
  aux properties;
};

/* [setProperty req property] sets the [property] in the [req] Request
   value */
let setProperty req property => {
  let reqData = Request.asJsonObject req;
  Js.Dict.set reqData property (Js.Json.boolean Js.true_);
};

/* return the string value for [key], None if the key is not in [dict]
   TODO once BOption.map is released */
let getDictString dict key => {
  switch (Js.Dict.get dict key) {
    | Some json => Js.Json.decodeString json
    | _ => None
  };
};

/* make a common JSON object representing success */
let makeSuccessJson () => {
  let json = Js.Dict.empty ();
  Js.Dict.set json "success" (Js.Json.boolean Js.true_);
  Js.Json.object_ json;
};

let app = express ();

App.useOnPath app path::"/" @@ Middleware.from (fun req _ next => {
  setProperty req "middleware0";
  next Next.middleware
    /* call the next middleware in the processing pipeline */
});

App.useWithMany app [|
  Middleware.from (fun req _ next => {
    checkProperty req next "middleware0" (fun () => {
      setProperty req "middleware1";
      next Next.middleware;
    });
  }),

  Middleware.from (fun req _ next => {
    checkProperties req next ["middleware0", "middleware1"] (fun () => {
      setProperty req "middleware2";
      next Next.middleware;
    });
  })
|];

App.get app path::"/" @@ Middleware.from (fun req res next => {
  let previousMiddlewares = [ "middleware0", "middleware1", "middleware2"];
  checkProperties req next previousMiddlewares (fun () => {
    Response.sendJson res (makeSuccessJson ());
  });
});

App.useOnPath app path::"/static" {
    let options = Static.defaultOptions ();
    Static.make "static" options |> Static.asMiddleware
};

App.postWithMany app path::"/:id/id" [|
  Middleware.from (fun req res next => {
    let previousMiddlewares = [ "middleware0", "middleware1", "middleware2"];
    checkProperties req next previousMiddlewares (fun () => {
      switch (getDictString (Request.params req) "id") {
        | Some "123" => Response.sendJson res (makeSuccessJson());
        | _ => next Next.route;
      };
    });
  })
|];

App.get app path::"/baseUrl" @@ Middleware.from (fun req res next => {
  switch (Request.baseUrl req) {
    | "" => Response.sendJson res (makeSuccessJson ());
    | _ => next Next.route;
  };
});

App.get app path::"/hostname" @@ Middleware.from (fun req res next => {
  switch (Request.hostname req) {
    | "localhost" => Response.sendJson res (makeSuccessJson ());
    | _ => next Next.route;
  };
});

App.get app path::"/ip" @@ Middleware.from (fun req res next => {
  switch (Request.ip req) {
    | "127.0.0.1" => Response.sendJson res (makeSuccessJson ());
    | s => {Js.log s; next Next.route;}
    /* TODO why is it printing ::1 */
  };
});

App.get app path::"/method" @@ Middleware.from (fun req res next => {
  switch (Request.method_ req) {
    | Request.Get => Response.sendJson res (makeSuccessJson ());
    | s => {Js.log s; next Next.route;}
  };
});

App.get app path::"/originalUrl" @@ Middleware.from (fun req res next => {
  switch (Request.originalUrl req) {
    | "/originalUrl" => Response.sendJson res (makeSuccessJson ());
    | s => {Js.log s; next Next.route;}
  };
});

App.get app path::"/path" @@ Middleware.from (fun req res next => {
  switch (Request.path req) {
    | "/path" => Response.sendJson res (makeSuccessJson ());
    | s => {Js.log s; next Next.route;}
  };
});

App.get app path::"/protocol" @@ Middleware.from (fun req res next => {
  switch (Request.protocol req) {
    | Request.Http => Response.sendJson res (makeSuccessJson ());
    | s => {Js.log s; next Next.route;}
  };
});

App.get app path::"/query" @@ Middleware.from (fun req res next => {
  switch (getDictString (Request.query req) "key") {
    | Some "value" => Response.sendJson res (makeSuccessJson ());
    | _ => next Next.route;
  };
});

App.getWithMany app path::"/accepts" [|
  Middleware.from (fun req _ next => {
    switch (Request.accepts req [|"audio/whatever" , "audio/basic" |]) {
      | Some "audio/basic" => next Next.middleware;
      | _ => next Next.route;
    };
  }),
  Middleware.from (fun req res next => {
    switch (Request.accepts req [| "text/css" |]) {
     | None => Response.sendJson res (makeSuccessJson ());
     | _ => next Next.route;
    };
  })
|];

App.getWithMany app path::"/accepts-charsets" [|
  Middleware.from (fun req _ next => {
    switch (Request.acceptsCharsets req [|"UTF-8" , "UTF-16" |]) {
      | Some "UTF-8" => next Next.middleware;
      | _ => next Next.route;
    };
  }),
  Middleware.from (fun req res next => {
    switch (Request.acceptsCharsets req [| "UTF-16" |]) {
     | None => Response.sendJson res (makeSuccessJson ());
     | _ => next Next.route;
    };
  })
|];

App.get app path::"/get" @@ Middleware.from (fun req res next => {
  switch (Request.get req "key") {
    | Some "value" => Response.sendJson res (makeSuccessJson ());
    | _ => next Next.route;
  };
});

App.get app path::"/fresh" @@ Middleware.from (fun req res next => {
  if(not @@ Request.fresh req) {
    Response.sendJson res (makeSuccessJson ());
  }
  else {
    next Next.route;
  };
});

App.get app path::"/stale" @@ Middleware.from (fun req res next => {
  if(Request.stale req) {
    Response.sendJson res (makeSuccessJson ());
  }
  else {
    next Next.route;
  };
});

App.get app path::"/secure" @@ Middleware.from (fun req res next => {
  if(not @@ Request.secure req) {
    Response.sendJson res (makeSuccessJson ());
  }
  else {
    next Next.route;
  };
});

App.get app path::"/xhr" @@ Middleware.from (fun req res next => {
  if(not @@ Request.xhr req) {
    Response.sendJson res (makeSuccessJson ());
  }
  else {
    next Next.route;
  };
});

App.get app path::"/redir" @@ Middleware.from (fun _req res _next =>
  Response.redirect res "/redir/target"
);

App.get app path::"/redircode" @@ Middleware.from (fun _req res _next =>
  Response.redirectCode res 301 "/redir/target"
);

let onListen port e =>
  switch e {
  | exception (Js.Exn.Error e) =>
    Js.log e;
    Node.Process.exit 1;
  | _ => Js.log @@ "Listening at http://127.0.0.1:" ^ (string_of_int port);
  };

App.listen app onListen::(onListen 3000) ();

/* Other examples are 
App.listen app (); 
App.listen app port::1000 ();
App.listen app port::1000 onListen::(fun e => Js.log e) ();
*/

/* -- Test the server --
npm run start && cd tests && ./test.sh
*/
