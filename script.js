// Campus Hub JavaScript Implementation
// Replicating the C++ functionality

class Student {
    constructor(name = "", rollNo = "") {
        this.name = name;
        this.rollNo = rollNo;
    }

    toString() {
        return `${this.name}|${this.rollNo}`;
    }

    static fromString(str) {
        const pos = str.indexOf('|');
        if (pos !== -1) {
            return new Student(str.substring(0, pos), str.substring(pos + 1));
        }
        return new Student();
    }
}

class ProjectGroup {
    constructor(projectTitle = "", className = "") {
        this.projectTitle = projectTitle;
        this.className = className;
        this.members = [];
    }

    addMember(student) {
        this.members.push(student);
    }

    hasStudentWithRoll(rollNo) {
        return this.members.some(student => student.rollNo === rollNo);
    }

    toString() {
        let result = `${this.projectTitle}~${this.className}~`;
        result += this.members.map(m => m.toString()).join(';');
        return result;
    }

    static fromString(str) {
        const pg = new ProjectGroup();
        const pos1 = str.indexOf('~');
        const pos2 = str.indexOf('~', pos1 + 1);

        if (pos1 !== -1 && pos2 !== -1) {
            pg.projectTitle = str.substring(0, pos1);
            pg.className = str.substring(pos1 + 1, pos2);

            const studentsStr = str.substring(pos2 + 1);
            if (studentsStr) {
                const studentStrings = studentsStr.split(';');
                pg.members = studentStrings.map(s => Student.fromString(s));
            }
        }
        return pg;
    }
}

class CampusHub {
    constructor() {
        this.groups = [];
        this.loadFromStorage();
    }

    addProject(projectTitle, className, students) {
        const pg = new ProjectGroup(projectTitle, className);
        students.forEach(s => {
            pg.addMember(new Student(s.name, s.rollNo));
        });
        this.groups.push(pg);
        // persist after add
        this.saveToStorage();
        this.updateStats();
        return true;
    }

    displayAll() {
        return this.groups;
    }

    linearSearchTitle(title) {
        for (let i = 0; i < this.groups.length; i++) {
            if (this.groups[i].projectTitle === title) {
                return i;
            }
        }
        return -1;
    }

    binarySearchTitle(title) {
        const tempGroups = [...this.groups];
        this.mergeSort(tempGroups, 0, tempGroups.length - 1, true);

        let left = 0, right = tempGroups.length - 1;
        while (left <= right) {
            const mid = Math.floor(left + (right - left) / 2);
            const midTitle = tempGroups[mid].projectTitle;

            if (midTitle === title) {
                return tempGroups[mid];
            } else if (midTitle < title) {
                left = mid + 1;
            } else {
                right = mid - 1;
            }
        }
        return null;
    }

    searchByRoll(rollNo) {
        const results = [];
        for (let i = 0; i < this.groups.length; i++) {
            if (this.groups[i].hasStudentWithRoll(rollNo)) {
                results.push(this.groups[i]);
            }
        }
        return results;
    }

    merge(arr, left, mid, right, sortByTitle) {
        const n1 = mid - left + 1;
        const n2 = right - mid;
        const L = new Array(n1);
        const R = new Array(n2);

        for (let i = 0; i < n1; i++) {
            L[i] = arr[left + i];
        }
        for (let j = 0; j < n2; j++) {
            R[j] = arr[mid + 1 + j];
        }

        let i = 0, j = 0, k = left;

        while (i < n1 && j < n2) {
            const leftVal = sortByTitle ? L[i].projectTitle : L[i].className;
            const rightVal = sortByTitle ? R[j].projectTitle : R[j].className;

            if (leftVal <= rightVal) {
                arr[k] = L[i];
                i++;
            } else {
                arr[k] = R[j];
                j++;
            }
            k++;
        }

        while (i < n1) {
            arr[k] = L[i];
            i++;
            k++;
        }

        while (j < n2) {
            arr[k] = R[j];
            j++;
            k++;
        }
    }

    mergeSort(arr, left, right, sortByTitle) {
        if (left < right) {
            const mid = Math.floor(left + (right - left) / 2);
            this.mergeSort(arr, left, mid, sortByTitle);
            this.mergeSort(arr, mid + 1, right, sortByTitle);
            this.merge(arr, left, mid, right, sortByTitle);
        }
    }

    sortByTitle() {
        if (this.groups.length === 0) return;
        this.mergeSort(this.groups, 0, this.groups.length - 1, true);
        // persist after sort
        this.saveToStorage();
        return this.groups;
    }

    sortByClassName() {
        if (this.groups.length === 0) return;
        this.mergeSort(this.groups, 0, this.groups.length - 1, false);
        // persist after sort
        this.saveToStorage();
        return this.groups;
    }

    saveToStorage() {
        try {
            const data = this.groups.map(g => g.toString());
            localStorage.setItem('campushub_data', JSON.stringify(data));
            return true;
        } catch (e) {
            return false;
        }
    }

    loadFromStorage() {
        try {
            const data = localStorage.getItem('campushub_data');
            if (data) {
                const groups = JSON.parse(data);
                this.groups = groups.map(g => ProjectGroup.fromString(g));
                this.updateStats();
            }
        } catch (e) {
            console.error('Error loading data:', e);
        }
    }

    getTotalGroups() {
        return this.groups.length;
    }

    updateStats() {
        const statsEl = document.getElementById('totalGroups');
        if (statsEl) {
            statsEl.textContent = this.getTotalGroups();
        }
    }

    clearData() {
        this.groups = [];
        this.saveToStorage();
        this.updateStats();
        return true;
    }
}

// Initialize CampusHub
const hub = new CampusHub();

// UI Functions
function renderDisplayAll() {
    const groups = hub.displayAll();
    const output = document.getElementById('display-output');
    if (!output) return;
    if (groups.length === 0) {
        output.innerHTML = '<div class="output info"><div class="empty-state">No project groups found in the system.</div></div>';
    } else {
        output.innerHTML = groups.map((g, i) => renderProjectGroup(g, i)).join('');
    }
}

function showSection(sectionId) {
    document.querySelectorAll('.section').forEach(s => s.classList.remove('active'));
    document.querySelectorAll('.menu-btn').forEach(b => b.classList.remove('active'));
    
    const section = document.getElementById(sectionId);
    const button = document.querySelector(`[data-action="${sectionId}"]`);
    
    if (section) section.classList.add('active');
    if (button) button.classList.add('active');

    // Auto-render when navigating to certain sections
    if (sectionId === 'display') {
        renderDisplayAll();
    }
}

function renderProjectGroup(group, index = null) {
    const indexText = index !== null ? `Group #${index + 1}` : '';
    return `
        <div class="project-group">
            <div class="project-group-header">
                <h3>${indexText}</h3>
                <p><strong>Project Title:</strong> ${group.projectTitle}</p>
                <p><strong>Class Name:</strong> ${group.className}</p>
            </div>
            <table class="students-table">
                <thead>
                    <tr>
                        <th>Student Name</th>
                        <th>Roll Number</th>
                    </tr>
                </thead>
                <tbody>
                    ${group.members.map(m => `
                        <tr>
                            <td>${m.name}</td>
                            <td>${m.rollNo}</td>
                        </tr>
                    `).join('')}
                </tbody>
            </table>
        </div>
    `;
}

function renderOutput(elementId, content, type = 'info') {
    const element = document.getElementById(elementId);
    if (element) {
        element.innerHTML = `<div class="output ${type}">${content}</div>`;
    }
}

// Event Listeners
document.querySelectorAll('.menu-btn').forEach(btn => {
    btn.addEventListener('click', () => {
        showSection(btn.dataset.action);
    });
});

// Add Project Form
document.getElementById('num-students').addEventListener('input', function() {
    const num = parseInt(this.value) || 0;
    const container = document.getElementById('students-container');
    container.innerHTML = '';

    for (let i = 0; i < num; i++) {
        container.innerHTML += `
            <div class="student-input-group">
                <h3>Student ${i + 1}</h3>
                <div class="form-group">
                    <label>Name:</label>
                    <input type="text" class="student-name" required>
                </div>
                <div class="form-group">
                    <label>Roll No:</label>
                    <input type="text" class="student-roll" required>
                </div>
            </div>
        `;
    }
});

document.getElementById('project-form').addEventListener('submit', function(e) {
    e.preventDefault();
    
    const title = document.getElementById('project-title').value;
    const className = document.getElementById('class-name').value;
    const numStudents = parseInt(document.getElementById('num-students').value);
    
    const students = [];
    const nameInputs = document.querySelectorAll('.student-name');
    const rollInputs = document.querySelectorAll('.student-roll');
    
    for (let i = 0; i < nameInputs.length; i++) {
        students.push({
            name: nameInputs[i].value,
            rollNo: rollInputs[i].value
        });
    }
    
    if (hub.addProject(title, className, students)) {
        renderOutput('add-output', '<strong>✓ Success!</strong> Project group added successfully!', 'success');
        this.reset();
        document.getElementById('students-container').innerHTML = '';
        // Navigate to Display All to show the newly added project
        showSection('display');
        renderDisplayAll();
    }
});

// Display All
const displayBtn = document.querySelector('[data-action="display"]');
if (displayBtn) {
    displayBtn.addEventListener('click', function() {
        renderDisplayAll();
    });
}

// Search by Title (Linear)
document.getElementById('search-title-linear-form').addEventListener('submit', function(e) {
    e.preventDefault();
    const title = document.getElementById('search-title-linear-input').value;
    const index = hub.linearSearchTitle(title);
    const output = document.getElementById('search-title-linear-output');
    
    if (index !== -1) {
        output.innerHTML = `<div class="output success"><strong>✓ Found</strong> (Using Linear Search):</div>` + 
                          renderProjectGroup(hub.groups[index]);
    } else {
        renderOutput('search-title-linear-output', `<strong>✗ Not Found</strong> No project with title "${title}" exists.`, 'error');
    }
});

// Search by Title (Binary)
document.getElementById('search-title-binary-form').addEventListener('submit', function(e) {
    e.preventDefault();
    const title = document.getElementById('search-title-binary-input').value;
    const result = hub.binarySearchTitle(title);
    const output = document.getElementById('search-title-binary-output');
    
    if (result) {
        output.innerHTML = `<div class="output success"><strong>✓ Found</strong> (Using Binary Search):</div>` + 
                          renderProjectGroup(result);
    } else {
        renderOutput('search-title-binary-output', `<strong>✗ Not Found</strong> No project with title "${title}" exists.`, 'error');
    }
});

// Search by Roll Number
document.getElementById('search-roll-form').addEventListener('submit', function(e) {
    e.preventDefault();
    const rollNo = document.getElementById('search-roll-input').value;
    const results = hub.searchByRoll(rollNo);
    const output = document.getElementById('search-roll-output');
    
    if (results.length > 0) {
        output.innerHTML = `<div class="output success"><strong>✓ Found</strong> Student with Roll No "${rollNo}" found in:</div>` + 
                          results.map((g, i) => renderProjectGroup(g, hub.groups.indexOf(g))).join('');
    } else {
        renderOutput('search-roll-output', `<strong>✗ Not Found</strong> No student with Roll No "${rollNo}" exists.`, 'error');
    }
});

// Sort by Title
document.getElementById('sort-title-btn').addEventListener('click', function() {
    const sorted = hub.sortByTitle();
    const output = document.getElementById('sort-title-output');
    
    if (sorted && sorted.length > 0) {
        output.innerHTML = `<div class="output success"><strong>✓ Success!</strong> Projects sorted by Title (A-Z) using Merge Sort!</div>` + 
                          sorted.map((g, i) => renderProjectGroup(g, i)).join('');
    } else {
        renderOutput('sort-title-output', 'No project groups to sort.', 'info');
    }
});

// Sort by Class
document.getElementById('sort-class-btn').addEventListener('click', function() {
    const sorted = hub.sortByClassName();
    const output = document.getElementById('sort-class-output');
    
    if (sorted && sorted.length > 0) {
        output.innerHTML = `<div class="output success"><strong>✓ Success!</strong> Projects sorted by Class Name (A-Z) using Merge Sort!</div>` + 
                          sorted.map((g, i) => renderProjectGroup(g, i)).join('');
    } else {
        renderOutput('sort-class-output', 'No project groups to sort.', 'info');
    }
});

// Download CSV
const downloadCsvBtn = document.getElementById('download-csv-btn');
if (downloadCsvBtn) {
    downloadCsvBtn.addEventListener('click', function() {
        const groups = hub.displayAll();
        if (!groups || groups.length === 0) {
            renderOutput('export-output', '<strong>Info</strong> No data to export.', 'info');
            return;
        }

        // CSV header
        const rows = [['GroupIndex','ProjectTitle','ClassName','StudentName','RollNumber']];
        groups.forEach((g, gi) => {
            if (g.members.length === 0) {
                rows.push([gi + 1, g.projectTitle, g.className, '', '']);
            } else {
                g.members.forEach(m => {
                    rows.push([gi + 1, g.projectTitle, g.className, m.name, m.rollNo]);
                });
            }
        });

        const csv = rows.map(r => r.map(field => {
            const s = String(field ?? '');
            // Escape quotes and wrap with quotes if needed
            if (/[",\n]/.test(s)) {
                return '"' + s.replace(/"/g, '""') + '"';
            }
            return s;
        }).join(',')).join('\n');

        const blob = new Blob([csv], { type: 'text/csv;charset=utf-8;' });
        const url = URL.createObjectURL(blob);
        const a = document.createElement('a');
        a.href = url;
        a.download = 'campushub.csv';
        document.body.appendChild(a);
        a.click();
        document.body.removeChild(a);
        URL.revokeObjectURL(url);

        renderOutput('export-output', '<strong>✓ Success!</strong> CSV downloaded as campushub.csv', 'success');
    });
}

// Save to Server (CSV)
const saveServerBtn = document.getElementById('save-server-btn');
if (saveServerBtn) {
    saveServerBtn.addEventListener('click', async function() {
        const groups = hub.displayAll();
        try {
            const resp = await fetch('http://localhost:3000/api/csv', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ groups })
            });
            if (!resp.ok) {
                const txt = await resp.text();
                renderOutput('export-output', `<strong>✗ Error</strong> Server error: ${txt}`, 'error');
                return;
            }
            const json = await resp.json();
            if (json && json.ok) {
                renderOutput('export-output', `<strong>✓ Success!</strong> Saved to ${json.path}`, 'success');
            } else {
                renderOutput('export-output', '<strong>✗ Error</strong> Unexpected server response.', 'error');
            }
        } catch (e) {
            renderOutput('export-output', `<strong>✗ Error</strong> Unable to reach server. Is it running on port 3000?`, 'error');
        }
    });
}

// Clear Data
const clearDataBtn = document.getElementById('clear-data-btn');
if (clearDataBtn) {
    clearDataBtn.addEventListener('click', async function() {
        if (!confirm('⚠️ WARNING: This will delete ALL project groups and clear all stored data!\n\nAre you sure you want to continue?')) {
            return;
        }

        // Clear local storage
        hub.clearData();

        // Clear server CSV file
        try {
            const resp = await fetch('http://localhost:3000/api/clear', {
                method: 'POST'
            });
            if (resp.ok) {
                const json = await resp.json();
                renderOutput('clear-data-output', `<strong>✓ Success!</strong> All project data has been cleared successfully!${json.message ? ' ' + json.message : ''}`, 'success');
            } else {
                // Even if server fails, local data is cleared
                renderOutput('clear-data-output', `<strong>✓ Success!</strong> Local data cleared. (Server may not be running, but local data is cleared.)`, 'success');
            }
        } catch (e) {
            // Even if server fails, local data is cleared
            renderOutput('clear-data-output', `<strong>✓ Success!</strong> Local data cleared. (Server may not be running, but local data is cleared.)`, 'success');
        }

        // Refresh display if on display page
        if (document.getElementById('display-all').classList.contains('active')) {
            renderDisplayAll();
        }
    });
}

// Removed Local Storage clear/save UI and logic per request

// Initialize
hub.updateStats();

