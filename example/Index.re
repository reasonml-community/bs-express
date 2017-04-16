open Express;

/* The tests below relies upon the ability to store in the Request 
   objects abritrary JSON properties. 

   Each middleware will both check that previous middleware 
   have been called by making properties exists in the Request object and 
   upon success will themselves adds anothe property to the Request. 

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
        | Some b when (b == Js.true_) => k ()
        | _ => next Next.route
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

App.useN app [|
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

App.listen app port::3000 ;

/* -- Test the server --
npm run start && cd tests && ./test.sh
*/
