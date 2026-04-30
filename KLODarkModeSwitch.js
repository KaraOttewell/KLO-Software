(function () {
    const root = document.body;

    // localStorage is blocked on file:// in some browsers — guard it
    function getSaved() {
        try { return localStorage.getItem("klo-theme"); } catch { return null; }
    }
    function setSaved(val) {
        try { localStorage.setItem("klo-theme", val); } catch {}
    }
    function removeSaved() {
        try { localStorage.removeItem("klo-theme"); } catch {}
    }

    // Apply saved theme on load
    const saved = getSaved();
    if (saved === "light") root.classList.add("theme-light");
    if (saved === "dark")  root.classList.add("theme-dark");

    // Attach click handlers
    document.querySelectorAll("#theme-toggle button").forEach(btn => {
        btn.addEventListener("click", () => {
            const mode = btn.dataset.theme;
            root.classList.remove("theme-light", "theme-dark");

            if (mode === "light") {
                root.classList.add("theme-light");
                setSaved("light");
            } else if (mode === "dark") {
                root.classList.add("theme-dark");
                setSaved("dark");
            } else {
                removeSaved();
            }
        });
    });
})();