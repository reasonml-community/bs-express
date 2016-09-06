open Express;

let __dirname: string = [%bs.raw "__dirname"];

let app = Express.express ();

Express.get app "/" (fun req res => Response.json res [%bs.obj {root: __dirname}]);

Express.use app (Express.static path::"__dirname");

Express.listen app 3000;
