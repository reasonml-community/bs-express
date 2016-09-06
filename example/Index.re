open Express;

let __dirname: string = [%bs.raw "__dirname"];

let app = Express.express ();

Express.get app "/" (fun req res => Express.json res [%bs.obj {root: __dirname}]);
