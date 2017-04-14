open Express;

/* The tests below relies upon the ability to store in the Request 
   objects abritrary JSON properties. 

   Each middleware will both check that previous middleware 
   have been called by making properties exists in the Request object and 
   upon success will themselves adds anothe property to the Request. 

*/

/* [check_property req next property k] makes sure [property] is 
   present in [req]. If success then [k()] is invoked, otherwise 
   [Next.route] is called with next */
let check_property req next property k => {
  let reqData = Request.asJsonObject req; 
  switch (Js_dict.get reqData property) {
    | None => next Next.route
    | Some x => {
      switch (Js_json.decodeBoolean x) {
        | Some b when (b == Js.true_) => k ()
        | _ => next Next.route
      };
    };
  };
};

/* same as [check_property] but with a list of properties */
let check_properties req next properties k => {
  let rec aux properties => {
    switch properties {
      | [] => k ();
      | [p, ...tl] => check_property req next p (fun () => aux tl); 
    }; 
  };
  aux properties;
};

/* [set_property req property] sets the [property] in the [req] Request 
   value */
let set_property req property => { 
  let reqData = Request.asJsonObject req; 
  Js_dict.set reqData property (Js_json.boolean Js.true_);
};

/* sends a common JSON object representing success */
let send_success_json res => {
  let json = Js_dict.empty (); 
  Js_dict.set json "success" (Js_json.boolean Js.true_); 
  Response.sendJson res (Js_json.object_ json);
};


let app = express ();

App.useOnPath app path::"/" @@ Middleware.from (fun req _ next => {
  set_property req "middleware0";
  next Next.middleware
    /* call the next middleware in the processing pipeline */
});

App.useN app [|
  Middleware.from (fun req _ next => {
    check_property req next "middleware0" (fun () => {
      set_property req "middleware1";
      next Next.middleware;
    }); 
  }),

  Middleware.from (fun req _ next => {
    check_properties req next ["middleware0", "middleware1"] (fun () => {
      set_property req "middleware2";
      next Next.middleware;
    }); 
  })
|]; 

App.get app path::"/" @@ Middleware.from (fun req res next => { 
  let prev_middlewares = [ "middleware0", "middleware1", "middleware2"];
  check_properties req next prev_middlewares (fun () => {
    send_success_json res;
  });
});

App.useOnPath app path::"/static" {
    let options = Static.defaultOptions (); 
    Static.make "static" options |> Static.asMiddleware 
};

App.postN app path::"/:id/id" [|
  Middleware.from (fun req res next => {
    let prev_middlewares = [ "middleware0", "middleware1", "middleware2"];
    check_properties req next prev_middlewares (fun () => {
      switch (Js_dict.get (Request.params req) "id") {
        | Some json => switch (Js_json.decodeString json) {
          | Some "123" => send_success_json res
          | _ => next Next.route;
        };
        | _ => next Next.route;
      };
    });
  })
|];

App.listen app port::3000 ;

/* -- Test the server --
 
Now run the following with to test the server:

curl http://localhost:3000/
> expect to see {"success":true}

curl http://localhost:3000/static/test.data
> expect to see the same output as `cat static/test.data`

curl -X POST http://localhost:3000/123/id
> expect to see {"success":true}

curl -X POST http://localhost:3000/999/id
> expect to see an error 

*/
