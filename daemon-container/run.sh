make daemon
build/daemon/nanoaltaird &
cd node
npm install .
node index.js
