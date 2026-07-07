const express = require("express");
const multer = require("multer");
const path = require("path");
const fs = require("fs");

const app = express();
const PORT = 3000;

// Upload folder configuration
const uploadFolder = path.join(__dirname, "uploads");

// Ensure upload directory exists asynchronously at startup
if (!fs.existsSync(uploadFolder)) {
    fs.mkdirSync(uploadFolder, { recursive: true });
}

// Multer storage pipeline
const storage = multer.diskStorage({
    destination: function (req, file, cb) {
        cb(null, uploadFolder);
    },
    filename: function (req, file, cb) {
        cb(null, Date.now() + "-" + file.originalname.replace(/[\s]/g, "_"));
    }
});

const upload = multer({ storage });

// Serve frontend assets cleanly
app.use(express.static("public"));
app.use(express.json({ limit: '50mb' })); // Increased payload limit just in case your images are large base64 strings!

// Multi-file upload endpoint
app.post("/upload", upload.array("files"), (req, res) => {
    if (!req.files || req.files.length === 0) {
        return res.status(400).json({ error: "No files uploaded" });
    }
    res.json({ message: "Files uploaded successfully!", files: req.files });
});

// List files asynchronously with formatted display names
app.get("/files", (req, res) => {
    fs.readdir(uploadFolder, (err, files) => {
        if (err) return res.status(500).json({ error: "Error reading folder content" });
        
        const formattedFiles = files.map(file => {
            const dashIndex = file.indexOf("-");
            const displayName = dashIndex !== -1 ? file.substring(dashIndex + 1) : file;
            
            return {
                realName: file,        
                displayName: displayName 
            };
        });
        
        res.json(formattedFiles);
    });
});

// Reading Text File
app.get('/text', (req, res) => {
    fs.readFile('Text.txt', 'utf8', (err, data) => {
        if (err) {
            if (err.code === 'ENOENT') return res.send(''); 
            return res.status(500).send('Error reading text file storage');
        }
        res.send(data);
    });
});

// Writing Text File Asynchronously
app.post("/text", (req, res) => {
    const newText = req.body.text || "";
    fs.writeFile('Text.txt', newText, 'utf8', err => {
        if (err) return res.sendStatus(500);
        return res.sendStatus(200);
    });
});

// Safe Download file routing using the real unique server name
app.get("/download/:name", (req, res) => {
    const file = path.join(uploadFolder, req.params.name);
    fs.access(file, fs.constants.F_OK, (err) => {
        if (err) return res.status(404).send("Requested file not found on server.");
        res.download(file);
    });
});

// Save HTML file (POCKET) - Non-Blocking Async
app.post("/pocket", (req, res) => {
    const htmlContent = req.body.html || "";
    fs.writeFile("Pocket.html", htmlContent, "utf8", (err) => {
        if (err) return res.status(500).send("Error compiling pocket changes");
        res.send("Saved");
    });
});

// Load HTML (POCKET)
app.get("/pocket", (req, res) => {
    fs.readFile("Pocket.html", "utf8", (err, data) => {
        if (err) {
            if (err.code === 'ENOENT') return res.send(''); 
            return res.status(500).send("Error reading pocket storage");
        }
        res.send(data);
    });
});

// Dynamic Port Binding Loop Engine
function tryToConnect(targetPort) {
    const server = app.listen(targetPort, () => {
        fs.writeFile('PORT.txt', String(targetPort), (err) => {
            if (err) console.error("Could not log port to file text metadata");
            console.log(`🚀 Stratosphere Engine active on: http://localhost:${targetPort}`);
        });
    });

    server.on('error', (err) => {
        if (err.code === 'EADDRINUSE') {
            console.log(`⚠️ Port ${targetPort} in use, attempting allocation shift upward...`);
            targetPort++;
            tryToConnect(targetPort);
        }
    });
}

tryToConnect(PORT);