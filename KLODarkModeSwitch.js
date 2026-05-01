(function () {
    const root = document.body;
    const icon = document.getElementById("theme-toggle-icon");
    if (!icon) return;

    const saved = localStorage.getItem("klo-theme");

    // Apply saved theme on load
    if (saved === "light") {
        root.classList.add("theme-light");
        icon.textContent = "🌞";
    }
    else if (saved === "dark") {
        root.classList.add("theme-dark");
        icon.textContent = "🌙";
    }
    else {
        icon.textContent = "🖥️"; // Auto
    }

    // Cycle through modes on click
    icon.addEventListener("click", () => {
        const current = localStorage.getItem("klo-theme");

        if (current === "light") {
            // Switch to dark
            root.classList.remove("theme-light");
            root.classList.add("theme-dark");
            localStorage.setItem("klo-theme", "dark");
            icon.textContent = "🌙";
        }
        else if (current === "dark") {
            // Switch to auto
            root.classList.remove("theme-dark");
            localStorage.removeItem("klo-theme");
            icon.textContent = "🖥️";
        }
        else {
            // Switch to light
            root.classList.add("theme-light");
            localStorage.setItem("klo-theme", "light");
            icon.textContent = "🌞";
        }
    });
})();
