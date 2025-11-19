async function fetchJSON(url, opts){
  const res = await fetch(url, Object.assign({ headers: { "Content-Type": "application/json" } }, opts));
  if(!res.ok) throw new Error((await res.json()).error||"error");
  return res.json();
}

function $(sel){return document.querySelector(sel)}

async function loadCaptcha(){
  const c = await fetchJSON("/api/captcha");
  $("#captchaText").textContent = c.code;
  $("#captchaToken").value = c.token;
}

if($("#loginForm")){
  loadCaptcha();
  $("#refreshCaptcha").addEventListener("click", loadCaptcha);
  $("#loginForm").addEventListener("submit", async (e)=>{
    e.preventDefault();
    $("#error").textContent = "";
    try{
      const form = new FormData(e.target);
      const body = Object.fromEntries(form.entries());
      const r = await fetchJSON("/api/login", { method: "POST", body: JSON.stringify(body) });
      if(r.role === "faculty") location.href = "/faculty.html"; else location.href = "/student.html";
    }catch(err){$("#error").textContent = err.message}
  });
  $("#registerForm").addEventListener("submit", async (e)=>{
    e.preventDefault();
    $("#regError").textContent = "";$("#regOk").textContent = "";
    try{
      const form = new FormData(e.target);
      const body = Object.fromEntries(form.entries());
      await fetchJSON("/api/register", { method: "POST", body: JSON.stringify(body) });
      $("#regOk").textContent = "Registered";
      e.target.reset();
    }catch(err){$("#regError").textContent = err.message}
  });
}

if($("#logoutBtn")){
  $("#logoutBtn").addEventListener("click", async ()=>{
    try{await fetchJSON("/api/logout", { method: "POST" }); location.href = "/";}catch{}
  });
}

async function loadFacultyProjects(){
  const list = await fetchJSON("/api/projects");
  const el = $("#projects");
  el.innerHTML = "";
  list.forEach((p)=>{
    const div = document.createElement("div");
    div.className = "list-item";
    div.innerHTML = `<div><strong>${p.title}</strong> <span class="badge">ID ${p.id}</span></div>
    <div>Class ${p.className} • Uploaded ${p.uploadDate}</div>
    <div>Students ${p.counts.total} • Viewed ${p.counts.viewed} • Submitted ${p.counts.submitted} • Completed ${p.counts.completed}</div>
    <div><a class="link" href="/${p.pdfFile}" target="_blank">Open PDF</a></div>`;
    el.appendChild(div);
  });
}

if($("#uploadForm")){
  loadFacultyProjects();
  $("#uploadForm").addEventListener("submit", async (e)=>{
    e.preventDefault();
    const fd = new FormData(e.target);
    const res = await fetch("/api/projects", { method: "POST", body: fd });
    if(res.ok){$("#uploadMsg").textContent = "Uploaded"; e.target.reset(); loadFacultyProjects();}
  });
  $("#loadStudents").addEventListener("click", async ()=>{
    const id = $("#detailProjectId").value.trim();
    const sort = $("#sortSelect").value;
    if(!id) return;
    const arr = await fetchJSON(`/api/projects/${id}/students?sort=${sort}`);
    const box = $("#students");
    box.innerHTML = "";
    arr.forEach((s)=>{
      const row = document.createElement("div");
      row.className = "list-item";
      row.innerHTML = `<div>${s.rollNo} • ${s.name} • Viewed ${s.viewed?"Yes":"No"} • Submitted ${s.submitted?"Yes":"No"}</div>
      <div><label><input type="checkbox" ${s.completed?"checked":""} data-roll="${s.rollNo}"> Completed</label></div>`;
      box.appendChild(row);
    });
    box.querySelectorAll("input[type=checkbox]").forEach((chk)=>{
      chk.addEventListener("change", async (e)=>{
        const roll = e.target.getAttribute("data-roll");
        const completed = e.target.checked;
        await fetchJSON(`/api/projects/${id}/students/${roll}/complete`, { method: "POST", body: JSON.stringify({ completed }) });
      });
    });
  });
}

async function loadStudentProjects(){
  const list = await fetchJSON("/api/projects");
  const el = $("#studentProjects");
  el.innerHTML = "";
  list.forEach((p)=>{
    const div = document.createElement("div");
    div.className = "list-item";
    div.innerHTML = `<div><strong>${p.title}</strong> <span class="badge">ID ${p.id}</span></div>
    <div>${p.description}</div>
    <div><a class="link" href="/${p.pdfFile}" target="_blank">Open PDF</a></div>`;
    el.appendChild(div);
  });
}

if($("#studentProjects")){
  loadStudentProjects();
  $("#viewProject").addEventListener("click", async ()=>{
    const id = $("#projectId").value.trim();
    if(!id) return;
    await fetchJSON(`/api/projects/${id}/view`, { method: "POST" });
    loadStudentProjects();
  });
  $("#submitForm").addEventListener("submit", async (e)=>{
    e.preventDefault();
    const fd = new FormData(e.target);
    const id = fd.get("id");
    const res = await fetch(`/api/projects/${id}/submit`, { method: "POST", body: fd });
    if(res.ok){$("#submitMsg").textContent = "Submitted"; e.target.reset(); loadStudentProjects();}
  });
}