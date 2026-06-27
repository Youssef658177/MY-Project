
async function fetchNurses() {
    try {
        const response = await fetch('/api/nurses');
        const nurses = await response.json();
        const list = document.getElementById('nursesList');
        list.innerHTML = '';
        nurses.forEach(nurse => {
            let li = document.createElement('li');
            li.innerText = `${nurse.name} - التخصص: ${nurse.specialty} (التقييم: ${nurse.rating})`;
            list.appendChild(li);
        });
    } catch (error) {
        console.error('Error:', error);
    }
}
