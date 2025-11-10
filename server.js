const express = require('express');
const cors = require('cors');
const fs = require('fs');
const path = require('path');

const app = express();
const PORT = process.env.PORT || 3000;

app.use(cors());
app.use(express.json({ limit: '2mb' }));

// Serve static frontend if desired (optional)
app.use(express.static(path.join(__dirname)));

// Ensure data directory exists
const dataDir = path.join(__dirname, 'data');
if (!fs.existsSync(dataDir)) {
    fs.mkdirSync(dataDir, { recursive: true });
}
const csvPath = path.join(dataDir, 'campushub.csv');

// Helper to convert groups to CSV
function groupsToCsv(groups) {
    const rows = [['GroupIndex','ProjectTitle','ClassName','StudentName','RollNumber']];
    (groups || []).forEach((g, gi) => {
        if (!g.members || g.members.length === 0) {
            rows.push([gi + 1, g.projectTitle || '', g.className || '', '', '']);
        } else {
            g.members.forEach(m => {
                rows.push([
                    gi + 1,
                    g.projectTitle || '',
                    g.className || '',
                    (m && m.name) || '',
                    (m && m.rollNo) || ''
                ]);
            });
        }
    });
    const csv = rows.map(r => r.map(field => {
        const s = String(field ?? '');
        return /[",\n]/.test(s) ? `"${s.replace(/"/g, '""')}"` : s;
    }).join(',')).join('\n');
    return csv;
}

// POST /api/csv - overwrite CSV with current groups
app.post('/api/csv', (req, res) => {
    try {
        const groups = req.body && req.body.groups;
        if (!Array.isArray(groups)) {
            return res.status(400).json({ ok: false, message: 'Invalid payload: groups must be an array' });
        }
        const csv = groupsToCsv(groups);
        fs.writeFileSync(csvPath, csv, 'utf8');
        return res.json({ ok: true, path: 'data/campushub.csv', bytes: Buffer.byteLength(csv, 'utf8') });
    } catch (err) {
        console.error('Error writing CSV:', err);
        return res.status(500).json({ ok: false, message: 'Failed to write CSV' });
    }
});

// Optional: GET /api/csv - returns file content (for verification)
app.get('/api/csv', (req, res) => {
    try {
        if (!fs.existsSync(csvPath)) {
            return res.status(404).json({ ok: false, message: 'CSV not found' });
        }
        res.setHeader('Content-Type', 'text/csv; charset=utf-8');
        fs.createReadStream(csvPath).pipe(res);
    } catch (err) {
        console.error('Error reading CSV:', err);
        res.status(500).json({ ok: false, message: 'Failed to read CSV' });
    }
});

app.listen(PORT, () => {
    console.log(`CampusHub backend running on http://localhost:${PORT}`);
});

