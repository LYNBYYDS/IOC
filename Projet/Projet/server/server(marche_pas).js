const http = require('http');
const { spawn } = require('child_process');
const path = require('path');

const port = 8000;

const requestHandler = (request, response) => {
  if (request.url === '/') {
    const scriptPath = path.join(__dirname, 'cgi-bin/main.py');
    const child = spawn(scriptPath, [], { shell: true });
    child.stdout.on('data', (data) => {
      response.write(data);
    });
    child.stderr.on('data', (data) => {
      console.error(`stderr: ${data}`);
    });
    child.on('close', (code) => {
      console.log(`child process exited with code ${code}`);
      response.end();
    });
  } else {
    response.end('Hello, world!');
  }
}

const server = http.createServer(requestHandler);

server.listen(port, (err) => {
  if (err) {
    return console.log('something bad happened', err)
  }

  console.log(`server is listening on ${port}`)
})
