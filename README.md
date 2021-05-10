# bs-express

Express bindings for [ReScript](https://github.com/rescript-lang/rescript-compiler) and [Reason](https://github.com/facebook/reason).

## Installing

1. Install `bs-express` using npm:

```
npm install --save bs-express
```

2. Add bs-express as a dependency to your `bsconfig.json`:

```json
{
  "name": "your-project",
  "bs-dependencies": ["bs-express"]
}
```

---

Right now the library is somewhat underdocumented, so please view the interface file [`Express.rei`](./src/Express.rei) or the [example folder](./example/) to see library usage.

---

## Contributing

If you'd like to contribute, you can follow the instructions below to get things working locally.

### Getting Started

1. After cloning the repo, install the dependencies

```shell
npm install
```

2. Build and start the example server:

```shell
npm start
```

### Running the tests

To run tests, run the command:

```shell
npm test
```
